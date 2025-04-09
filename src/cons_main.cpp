#include <boost/program_options.hpp>

#include "log_utils.hpp"
#include "consumer.hpp"
#include "socket_handler.hpp"
#include "ahrs_processor.hpp"

namespace po = boost::program_options;
namespace logging = boost::log;

void parse_command_line_options(int argc, char* argv[],
                              std::string& socket_path,
                              std::string& log_level_string,
                              int& timeout_ms) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("socket-path,s", po::value<std::string>(&socket_path)->required(), 
         "socket path")
        ("log-level,l", po::value<std::string>(&log_level_string), 
         "log level (trace, debug, info, warning, error, fatal)")
        ("timeout,t", po::value<int>(&timeout_ms), 
         "timeout in milliseconds");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        throw std::runtime_error("Help requested");
    }

    po::notify(vm);
}

int main(int argc, char* argv[]) {
    std::string socket_path;
    std::string log_level_string = "info";
    int timeout_ms = 1000;

    try {
        parse_command_line_options(argc, argv, socket_path, log_level_string, timeout_ms);
        
        auto log_level = parse_log_level(log_level_string);
        init_logging(log_level);

        auto socket_receiver = std::make_unique<SocketReceiver>(socket_path);
        if (!socket_receiver->initialize()) {
            return EXIT_FAILURE;
        }

        auto ahrs_processor = std::make_unique<AhrsProcessor>();
        ahrs_processor->initialize();

        Consumer consumer(std::move(socket_receiver), 
                        std::move(ahrs_processor),
                        timeout_ms);
        
        consumer.run();

    } catch (const std::exception& ex) {
        BOOST_LOG_TRIVIAL(error) << "Error: " << ex.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}