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

#include "transform.hpp"

namespace shijima {
namespace action {

bool transform::tick() {
    bool ret = animate::tick();
    if (animation_finished()) {
        // Animation concluded, create a breed request
        // It is up to the library consumer to comply with the request

        auto &request = mascot->breed_request;
        request.available = true;
        request.behavior = vars.get_string("TransformBehavior", "Fall");
        request.name = vars.get_string("TransformMascot", "");
        request.transient = false; //FIXME: should this be true?
        
        request.anchor = { mascot->anchor.x, mascot->anchor.y };

        // This is a transformation so this shimeji should disappear now
        mascot->dead = true;

        return false;
    }
    return ret;
}

}
}