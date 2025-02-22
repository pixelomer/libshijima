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

#include "jump.hpp"

namespace shijima {
namespace action {

bool jump::tick() {
    math::vec2 target { vars.get_num("TargetX", 0),
        vars.get_num("TargetY", 0) };
    mascot->looking_right = mascot->anchor.x < target.x;
    math::vec2 distance;
    distance.x = target.x - mascot->anchor.x,
    distance.y = target.y - mascot->anchor.y - std::abs(distance.x);
    double velocity_abs = vars.get_num("VelocityParam", 20);
    double distance_abs = sqrtf(distance.x*distance.x + distance.y*distance.y);
    if (distance_abs != 0) {
        math::vec2 velocity { velocity_abs * distance.x / distance_abs,
            velocity_abs * distance.y / distance_abs };
        mascot->anchor.x += velocity.x;
        mascot->anchor.y += velocity.y;
    }
    if (distance_abs <= velocity_abs) {
        mascot->anchor = target;
        return false;
    }
    return animation::tick();
}

}
}