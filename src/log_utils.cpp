#include "log_utils.h"

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
        keywords::file_name = "consumer_%N.log",
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
