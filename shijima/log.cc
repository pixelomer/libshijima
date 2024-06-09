#include "log.hpp"

namespace shijima {

#if defined(SHIJIMA_LOGGING_ENABLED)

static uint16_t log_level = 0;

void set_log_level(uint16_t level) {
    log_level = level;
}
uint16_t get_log_level() {
    return log_level;
}
void log(uint16_t level, std::string const& log) {
    if (log_level & level) {
        std::cerr << log << std::endl;
    }
}
void log(std::string const& log) {
    std::cerr << log << std::endl;
}

#endif /* defined(SHIJIMA_LOGGING_ENABLED) */

}