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

#include "state.hpp"
#include <shijima/log.hpp>

namespace shijima {
namespace mascot {

void state::roll_dcursor() {
    int roller_size = env->subtick_count + 1;
    if ((int)stored_dcursor_data.size() != roller_size) {
        stored_dcursor = { 0, 0, 0, 0 };
        stored_dcursor_data.resize(roller_size);
        for (auto &vec : stored_dcursor_data) {
            vec = { 0, 0 };
        }
        next_dcursor_roll = 0;
    }
    
    auto raw_cursor = get_raw_cursor();
    if (dragging) {
        stored_dcursor_data[next_dcursor_roll] = { raw_cursor.dx,
            raw_cursor.dy };
    }
    else {
        stored_dcursor_data[next_dcursor_roll] = { 0, 0 };
    }
    
    stored_dcursor = { raw_cursor.x, raw_cursor.y, 0, 0 };
    for (auto const& vec : stored_dcursor_data) {
        stored_dcursor.dx += vec.x;
        stored_dcursor.dy += vec.y;
    }
    next_dcursor_roll = (next_dcursor_roll + 1) % roller_size;
}

environment::dvec2 &state::get_raw_cursor() {
    if (dragging && drag_with_local_cursor) {
        return local_cursor;
    }
    else {
        return env->cursor;
    }
}

environment::dvec2 state::get_cursor() {
    return stored_dcursor;
}

bool state::on_land() {
    return env->floor.is_on(anchor) ||
        env->ceiling.is_on(anchor) ||
        env->work_area.is_on(anchor) ||
        env->active_ie.is_on(anchor);
}

void state::warn(std::string const& msg) {
    #ifdef SHIJIMA_LOGGING_ENABLED
        shijima::log(SHIJIMA_LOG_WARNINGS, msg);
    #endif
    if (warnings_enabled) {
        warnings.push(msg);
    }
}
void state::warn(const char *msg) {
    #ifdef SHIJIMA_LOGGING_ENABLED
        shijima::log(SHIJIMA_LOG_WARNINGS, msg);
    #endif
    if (warnings_enabled) {
        warnings.push(msg);
    }
}

}
}
