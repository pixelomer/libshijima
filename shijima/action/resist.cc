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

#include "resist.hpp"

namespace shijima {
namespace action {

bool resist::handle_dragging() {
    if (!mascot->dragging) {
        // Stopped dragging
        mascot->queued_behavior = "Thrown";
        mascot->was_on_ie = false;
        return false;
    }
    return true;
}

void resist::init(mascot::tick &ctx) {
    animate::init(ctx);
    start_cursor = mascot->get_cursor();
}

bool resist::tick() {
    if (std::abs(mascot->get_cursor().x - start_cursor.x) >= 5) {
        // Cursor moved, abort without escaping
        mascot->queued_behavior = "Dragged";
        mascot->was_on_ie = false;
        return false;
    }
    bool ret = animate::tick();
    if (animation_finished()) {
        // Animation concluded, escape
        mascot->dragging = false;
        return false;
    }
    return ret;
}

}
}