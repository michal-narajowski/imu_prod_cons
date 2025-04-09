#include <boost/program_options.hpp>

#include "log_utils.hpp"
#include "consumer.hpp"
#include "socket_handler.hpp"
#include "ahrs_processor.hpp"
#include "rt_utils.hpp"

namespace po = boost::program_options;
namespace logging = boost::log;

void parse_command_line_options(int argc, char* argv[],
                              std::string& socket_path,
                              std::string& log_level_string,
                              int& timeout_ms,
                              bool& enable_rt) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("socket-path,s", po::value<std::string>(&socket_path)->required(), 
         "socket path")
        ("log-level,l", po::value<std::string>(&log_level_string), 
         "log level (trace, debug, info, warning, error, fatal)")
        ("timeout,t", po::value<int>(&timeout_ms), 
         "timeout in milliseconds")
        ("enable-rt,r", po::bool_switch(&enable_rt)->default_value(false),
         "enable real-time scheduling");

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
    bool enable_rt = false;

    try {
        parse_command_line_options(argc, argv, socket_path, log_level_string, timeout_ms, enable_rt);
        
        auto log_level = parse_log_level(log_level_string);
        init_logging(log_level);

        if (enable_rt) {
            BOOST_LOG_TRIVIAL(info) << "Enabling RT scheduling";
            RtUtils::verify_rt_privileges();
            RtUtils::set_rt_priority(70);
        }

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