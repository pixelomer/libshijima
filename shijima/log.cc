#include "log.hpp"
#include <iostream>

namespace shijima {

#ifdef SHIJIMA_LOGGING_ENABLED

void default_log(std::string const& log);

static uint16_t log_level = 0;
static std::function<void(std::string const&)> active_logger = default_log;

void set_log_level(uint16_t level) {
    log_level = level;
}
uint16_t get_log_level() {
    return log_level;
}
void log(uint16_t level, std::string const& log) {
    if (log_level & level) {
        active_logger(log);
    }
}
void log(std::string const& msg) {
    active_logger(msg);
}
void default_log(std::string const& log) {
    std::cerr << log << std::endl;
}
void set_logger(std::function<void(std::string const&)> logger) {
    active_logger = logger;
}

#endif /* defined(SHIJIMA_LOGGING_ENABLED) */

}