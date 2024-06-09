#pragma once
#include <string>
#include <iostream>
#include <cinttypes>

#define SHIJIMA_LOG_JAVASCRIPT 0x1
#define SHIJIMA_LOG_ACTIONS 0x2
#define SHIJIMA_LOG_BEHAVIORS 0x4
#define SHIJIMA_LOG_EVERYTHING 0xFFFF

#if defined(DEBUG)
#define SHIJIMA_LOGGING_ENABLED
#endif

namespace shijima {

#if defined(SHIJIMA_LOGGING_ENABLED)

void set_log_level(uint16_t level);
uint16_t get_log_level();
void log(uint16_t level, std::string const& log);
void log(std::string const& log);

#else

#define set_log_level(...)
#define get_log_level() (0)
#define log(...)

#endif /* defined(SHIJIMA_LOGGING_ENABLED) */

};