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
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>

#include "payload.h"

namespace po = boost::program_options;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;

logging::trivial::severity_level parse_log_level(const std::string& level) {
    if (level == "trace") return logging::trivial::trace;
    if (level == "debug") return logging::trivial::debug;
    if (level == "info") return logging::trivial::info;
    if (level == "warning") return logging::trivial::warning;
    if (level == "error") return logging::trivial::error;
    if (level == "fatal") return logging::trivial::fatal;
    throw std::runtime_error("Invalid log level: " + level);
}

void init_logging(logging::trivial::severity_level level) {
    logging::add_file_log(
        keywords::file_name = "producer_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::format = "[%TimeStamp%][%Severity%] %Message%"
    );

    logging::add_console_log(
        std::cout,
        keywords::format = "[%TimeStamp%][%Severity%] %Message%"
    );

    logging::core::get()->set_filter(
        logging::trivial::severity >= level
    );

    logging::add_common_attributes();
}

int main(int argc, char* argv[]) {
    std::string socket_path;
    int frequency_hz;
    std::string log_level_string = "info";

    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("socket-path,s", po::value<std::string>(&socket_path)->required(), "socket path")
            ("frequency-hz,f", po::value<int>(&frequency_hz)->required(), "frequency in Hz")
            ("log-level,l", po::value<std::string>(&log_level_string), "log level (trace, debug, info, warning, error, fatal)")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return EXIT_SUCCESS;
        }

        po::notify(vm);
    } catch (const po::error& ex) {
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
        perror("socket");
        return EXIT_FAILURE;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_float(-100.0f, 100.0f);
    std::uniform_int_distribution<int32_t> dist_int(-100, 100);
    std::uniform_int_distribution<uint32_t> dist_uint(0, 1000);

    while (true) {
        Payload_IMU_t packet;
        packet.xAcc = dist_float(gen);
        packet.yAcc = dist_float(gen);
        packet.zAcc = dist_float(gen);
        packet.timestampAcc = dist_uint(gen);
        packet.xGyro = dist_int(gen);
        packet.yGyro = dist_int(gen);
        packet.zGyro = dist_int(gen);
        packet.timestampGyro = dist_uint(gen);
        packet.xMag = dist_float(gen);
        packet.yMag = dist_float(gen);
        packet.zMag = dist_float(gen);
        packet.timestampMag = dist_uint(gen);

        if (sendto(sockfd, &packet, sizeof(packet), 0, (sockaddr*)&addr, sizeof(addr)) == -1) {
            perror("sendto");
            close(sockfd);
            return EXIT_FAILURE;
        }

        BOOST_LOG_TRIVIAL(debug) << "Sent packet: xAcc=" << packet.xAcc << ", yAcc=" << packet.yAcc << ", zAcc=" << packet.zAcc
                  << ", tsAcc=" << packet.timestampAcc << ", xGyro=" << packet.xGyro << ", yGyro=" << packet.yGyro
                  << ", zGyro=" << packet.zGyro << ", tsGyro=" << packet.timestampGyro << ", xMag=" << packet.xMag
                  << ", yMag=" << packet.yMag << ", zMag=" << packet.zMag << ", tsMag=" << packet.timestampMag;

        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }

    close(sockfd);
    return EXIT_SUCCESS;
}