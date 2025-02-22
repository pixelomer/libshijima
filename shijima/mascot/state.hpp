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

#include <memory>
#include <shijima/math.hpp>
#include <shijima/pose.hpp>
#include <shijima/broadcast/interaction.hpp>
#include "environment.hpp"

namespace shijima {
namespace mascot {

class state {
public:
    class breed_request_data {
    public:
        bool available = false;
        bool transient;
        bool looking_right;
        math::vec2 anchor;
        std::string name; // empty if self
        std::string behavior;
    };

    breed_request_data breed_request;

    math::rec bounds;
    math::vec2 anchor;
    shijima::frame active_frame;
    std::shared_ptr<environment> env;
    std::map<std::string, std::string> constants;
    broadcast::interaction interaction;
    std::string queued_behavior;
    std::string active_sound;
    bool active_sound_changed;
    bool looking_right;
    bool dragging;
    bool was_on_ie;
    bool dead;
    long time;
    bool can_breed = true;
    int next_subtick = 0;

    bool drag_with_local_cursor;
    environment::dvec2 local_cursor;

    std::vector<math::vec2> stored_dcursor_data;
    environment::dvec2 stored_dcursor;
    int next_dcursor_roll;
    math::vec2 active_ie_offset = { 0, 0 };

    void roll_dcursor();
    environment::dvec2 &get_raw_cursor();
    environment::dvec2 get_cursor();
    bool on_land();
};

}
}
