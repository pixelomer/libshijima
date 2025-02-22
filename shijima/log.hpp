#pragma once

// 
// libshijima - C++ library for shimeji desktop mascots
// Copyright (C) 2024-2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#include <string>
#include <cstdint>
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

}
