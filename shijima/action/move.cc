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

#include "move.hpp"
#include <shijima/log.hpp>

namespace shijima {
namespace action {

// "目的地Y" and "目的地X" are required for backwards compatibility

bool move::tick() {
    if (vars.has("TargetX")) {
        double x = vars.get_num("TargetX");
        vars.add_attr({{ "目的地X", x }});
        auto &pose = get_pose();
        if (pose.velocity.x > 0) {
            mascot->looking_right = (x < mascot->anchor.x);
        }
        else if (pose.velocity.x < 0) {
            mascot->looking_right = (x > mascot->anchor.x);
        }
    }
    if (vars.has("TargetY")) {
        vars.add_attr({{ "目的地Y", vars.get_num("TargetY") }});
        if (mascot->env->work_area.left_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.right_border().is_on(mascot->anchor))
        {
            mascot->looking_right = false;
        }
        if (mascot->env->work_area.right_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.left_border().is_on(mascot->anchor))
        {
            mascot->looking_right = true;
        }
    }

    auto start = mascot->anchor;
    if (!animation::tick()) {
        return false;
    }
    auto end = mascot->anchor;
    #define passed(x) (start.x >= x && end.x <= x) || \
        (start.x <= x && end.x >= x)
    if (vars.has("TargetX")) {
        double x = vars.get_num("TargetX");
        if (passed(x)) {
            mascot->anchor.x = x;
            return false;
        }
    }
    else if (vars.has("TargetY")) {
        double y = vars.get_num("TargetY");
        if (passed(y)) {
            mascot->anchor.y = y;
            return false;
        }
    }
    else {
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_WARNINGS, "warning: neither TargetX nor TargetY defined");
        #endif
        return false;
    }
    return true;
}

}
}