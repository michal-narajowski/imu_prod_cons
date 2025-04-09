#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctime>
#include <chrono>
#include <thread>
#include <random>
#include <boost/program_options.hpp>

#include "log_utils.h"
#include "imu_data.h"
#include "random_imu_data.h"
#include "csv_imu_data.h"
#include "imu_data_factory.h"
#include "concrete_imu_data_factory.h"

namespace po = boost::program_options;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;

int main(int argc, char* argv[]) {
    std::string socket_path;
    int frequency_hz;
    std::string log_level_string = "info";
    std::string data_source = "random"; // Default data source
    std::string csv_file_path;

    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("socket-path,s", po::value<std::string>(&socket_path)->required(), "socket path")
            ("frequency-hz,f", po::value<int>(&frequency_hz)->required(), "frequency in Hz")
            ("log-level,l", po::value<std::string>(&log_level_string), "log level (trace, debug, info, warning, error, fatal)")
            ("data-source,d", po::value<std::string>(&data_source)->default_value("random"), "Data source (random or csv)")
            ("csv-file,c", po::value<std::string>(&csv_file_path), "CSV file path")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return EXIT_SUCCESS;
        }

        if (data_source == "csv" && !vm.count("csv-file")) {
            throw std::runtime_error("CSV file path must be specified when using CSV data source.");
        }
    } catch (const po::error& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }  catch (const std::runtime_error& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    logging::trivial::severity_level log_level;
    try {
        log_level = parse_log_level(log_level_string);
    } catch (const std::runtime_error& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    init_logging(log_level);

    int interval_ms = 1000 / frequency_hz;

    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create socket: " << strerror(errno);
        return EXIT_FAILURE;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    ConcreteImuDataFactory factory;
    std::unique_ptr<IImuDataProvider> data_provider = nullptr;
    std::random_device random_device;

    if (data_source == "random") {
        data_provider = factory.create_random_imu_data(random_device());
    } else if (data_source == "csv") {
        data_provider = factory.create_csv_imu_data(csv_file_path);
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error: Invalid data source specified.";
        return EXIT_FAILURE;
    }

    if (!data_provider->initialize()) {
        BOOST_LOG_TRIVIAL(error) << "Error: Failed to initialize data provider.";
        close(sockfd);
        return EXIT_FAILURE;
    }

    while (true) {
        ImuData_t packet = data_provider->get_next();

        if (sendto(sockfd, &packet, sizeof(packet), 0, (sockaddr*)&addr, sizeof(addr)) == -1) {
            BOOST_LOG_TRIVIAL(error) << "Failed to send data: " << strerror(errno);
            close(sockfd);
            return EXIT_FAILURE;
        }

        BOOST_LOG_TRIVIAL(debug) << "Sent packet: " << packet.to_string();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }

    close(sockfd);
    return EXIT_SUCCESS;
}