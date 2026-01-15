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

#include "offset.hpp"

namespace shijima {
namespace action {

bool offset::tick() {
    auto old_anchor = mascot->anchor;
    int dx = (int)vars.get_num("X", 0), dy = (int)vars.get_num("Y", 0);
    mascot->anchor.x += dx;
    mascot->anchor.y += dy;
    if (dx == 0 && dy < 0 &&
        ((mascot->env->work_area.left_border().is_on(old_anchor) &&
        mascot->env->work_area.left_border().is_on(mascot->anchor)) ||
        (mascot->env->work_area.right_border().is_on(old_anchor) &&
         mascot->env->work_area.right_border().is_on(mascot->anchor))) &&
        mascot->env->ceiling.is_on(mascot->anchor) &&
        !mascot->env->ceiling.is_on(old_anchor))
    {
        //XXX: HACK: ensure that after Offset Y=-64, the ceiling action is triggered
        //           (or, if the shimeji does not support ceiling movement, the shimeji falls)
        if (mascot->env->work_area.right_border().is_on(mascot->anchor)) {
            mascot->anchor.x -= 1.1;
        }
        else {
            mascot->anchor.x += 1.1;
        }
    }
    return false;
}

}
}