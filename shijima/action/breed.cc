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

#include "breed.hpp"

namespace shijima {
namespace action {

bool breed::tick() {
    bool transient = vars.get_bool("BornTransient", false);
    if (!transient && (!mascot->env->allows_breeding || !mascot->can_breed)) {
        return false;
    }
    bool ret = animate::tick();
    if (animation_finished()) {
        // Animation concluded, create a breed request
        // It is up to the library consumer to comply with the request

        auto &request = mascot->breed_request;
        double born_x = vars.get_num("BornX", 0);
        double born_y = vars.get_num("BornY", 0);
        request.available = true;
        request.behavior = vars.get_string("BornBehavior", "Fall");
        request.name = vars.get_string("BornMascot", "");
        request.transient = transient;
        
        request.anchor = { mascot->anchor.x + dx(born_x),
            mascot->anchor.y + dy(born_y) };

        return false;
    }
    return ret;
}

}
}