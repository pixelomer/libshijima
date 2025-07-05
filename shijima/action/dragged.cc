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

#include "dragged.hpp"

namespace shijima {
namespace action {

void dragged::init(mascot::tick &ctx) {
    animation::init(ctx);
    auto offset_x = vars.get_num("OffsetX", 0);
    foot_dx = 0;
    foot_x = mascot->get_cursor().x + offset_x;
    time_to_resist = 250;
    vars.add_attr({ { "FootX", foot_x }, { "footX", foot_x } });
    if (mascot->hotspot_triggered) {
        // Special case: Dragged action started by hotspot
        // Retain dragging flag
        mascot->dragging = true;
    }
}

bool dragged::requests_interpolation() {
    return false;
}

bool dragged::handle_dragging() {
    if (!mascot->dragging) {
        // Stopped dragging
        mascot->queued_behavior = "Thrown";
        mascot->was_on_ie = false;
        return false;
    }
    return true;
}

bool dragged::subtick(int idx) {
    mascot->looking_right = false;
    auto cursor = mascot->get_cursor();
    if (idx == 0) {
        foot_dx = (foot_dx + ((cursor.x - foot_x) * 0.1)) * 0.8;
        foot_x += foot_dx;
        vars.add_attr({ { "FootX", foot_x }, { "footX", foot_x },
            { "FootDX", foot_dx }, { "footDX", foot_dx } });
    }
    auto offset_x = vars.get_num("OffsetX", 0);
    auto offset_y = vars.get_num("OffsetY", 120);
    auto subtick_count = mascot->env->subtick_count;
    if (std::abs(cursor.x - mascot->anchor.x + offset_x) >= (5.0 / subtick_count)) {
        reset_elapsed();
    }
    if (!animation::subtick(idx)) {
        return false;
    }
    if (mascot->dragging) {
        mascot->anchor.x = cursor.x + offset_x;
        mascot->anchor.y = cursor.y + offset_y;
    }
    if (elapsed() >= time_to_resist) {
        return false;
    }
    return true;
}

}
}