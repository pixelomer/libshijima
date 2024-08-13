#pragma once
#include <string>
#include <iostream>
#include <cinttypes>
#include <functional>
#include "config.hpp"

#define SHIJIMA_LOG_JAVASCRIPT 0x1
#define SHIJIMA_LOG_ACTIONS 0x2
#define SHIJIMA_LOG_BEHAVIORS 0x4
#define SHIJIMA_LOG_WARNINGS 0x8
#define SHIJIMA_LOG_BROADCASTS 0x10
#define SHIJIMA_LOG_PARSER 0x20
#define SHIJIMA_LOG_EVERYTHING 0xFFFF

namespace shijima {

#ifdef SHIJIMA_LOGGING_ENABLED

void set_log_level(uint16_t level);
uint16_t get_log_level();
void log(uint16_t level, std::string const& log);
void log(std::string const& log);
void set_logger(std::function<void(std::string const&)> logger);

#endif

};