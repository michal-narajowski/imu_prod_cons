#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>

namespace logging = boost::log;

logging::trivial::severity_level parse_log_level(const std::string& level);
void init_logging(logging::trivial::severity_level level);
