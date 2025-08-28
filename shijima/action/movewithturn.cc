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

#include "movewithturn.hpp"

namespace shijima {
namespace action {

bool movewithturn::headed_right() {
    double target_x = vars.get_num("TargetX");
    return mascot->anchor.x <= target_x;
}

bool movewithturn::needs_turn() {
    return (mascot->looking_right && !headed_right()) ||
        (!mascot->looking_right && headed_right());
}

std::shared_ptr<shijima::animation> movewithturn::get_animation() {
    if (animations.size() < 2) {
        return move::get_animation();
    }
    if (!is_turning && needs_turn()) {
        is_turning = true;
        mascot->looking_right = headed_right();
        start_time = mascot->time;
    }
    else if (is_turning && elapsed() >= animations[1]->get_duration()) {
        is_turning = false;
        start_time = mascot->time;
    }
    return animations[is_turning ? 1 : 0];
}

void movewithturn::init(mascot::tick &ctx) {
    move::init(ctx);
    if (animations.size() != 2) {
        #ifndef SHIJIMA_LOGGING_ENABLED
        if (mascot->warnings_enabled)
        #endif
            mascot->warn(
                "expected 2 animations for MoveWithTurn, got "
                + std::to_string(animations.size()) + " animations");
    }
}

}
}