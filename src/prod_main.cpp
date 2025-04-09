#include <random>
#include <boost/program_options.hpp>

#include "log_utils.hpp"
#include "concrete_imu_data_factory.hpp"
#include "producer.hpp"
#include "rt_utils.hpp"

namespace po = boost::program_options;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void parse_command_line_options(int argc, char* argv[],
                              std::string& socket_path,
                              int& frequency_hz,
                              std::string& log_level_string,
                              std::string& data_source,
                              std::string& csv_file_path,
                              bool& enable_rt) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("socket-path,s", po::value<std::string>(&socket_path)->required(), 
         "socket path")
        ("frequency-hz,f", po::value<int>(&frequency_hz)->required(), 
         "frequency in Hz")
        ("log-level,l", po::value<std::string>(&log_level_string), 
         "log level (trace, debug, info, warning, error, fatal)")
        ("data-source,d", po::value<std::string>(&data_source)->default_value("random"), 
         "Data source (random or csv)")
        ("csv-file,c", po::value<std::string>(&csv_file_path), 
         "CSV file path")
        ("enable-rt,r", po::bool_switch(&enable_rt)->default_value(false),
         "enable real-time scheduling");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        throw std::runtime_error("Help requested");
    }

    po::notify(vm);

    if (data_source == "csv" && csv_file_path.empty()) {
        throw std::runtime_error("CSV file path must be specified when using CSV data source");
    }
}

int main(int argc, char* argv[]) {
    std::string socket_path;
    int frequency_hz;
    std::string log_level_string = "info";
    std::string data_source = "random";
    std::string csv_file_path;
    bool enable_rt = false;

    try {
        parse_command_line_options(argc, argv, socket_path, frequency_hz, 
                                 log_level_string, data_source, csv_file_path, enable_rt);
        
        auto log_level = parse_log_level(log_level_string);
        init_logging(log_level);

        if (enable_rt) {
            BOOST_LOG_TRIVIAL(info) << "Enabling RT scheduling";
            RtUtils::verify_rt_privileges();
            RtUtils::set_rt_priority(80);
        }

        auto socket_sender = std::make_unique<SocketSender>(socket_path);
        if (!socket_sender->initialize()) {
            return EXIT_FAILURE;
        }

        ConcreteImuDataFactory factory;
        std::unique_ptr<IImuDataProvider> data_provider;
        
        try {
            if (data_source == "random") {
                data_provider = factory.create_random_imu_data(std::random_device()());
            } else if (data_source == "csv") {
                data_provider = factory.create_csv_imu_data(csv_file_path);
            } else {
                throw std::runtime_error("Invalid data source specified");
            }
        } catch (const std::exception& ex) {
            BOOST_LOG_TRIVIAL(error) << "Error creating data provider: " << ex.what();
            return EXIT_FAILURE;
        }

        if (!data_provider->initialize()) {
            BOOST_LOG_TRIVIAL(error) << "Failed to initialize data provider";
            return EXIT_FAILURE;
        }

        Producer producer(std::move(socket_sender), 
                        std::move(data_provider), 
                        frequency_hz);
        producer.run();

    } catch (const std::exception& ex) {
        BOOST_LOG_TRIVIAL(error) << "Error: " << ex.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}