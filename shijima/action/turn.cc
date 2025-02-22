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

#include "turn.hpp"

namespace shijima {
namespace action {

bool turn::tick() {
    bool will_look_right = vars.get_bool("LookRight", false);
    if (elapsed() == 0) {
        // First tick
        if ((will_look_right && mascot->looking_right) ||
            (!will_look_right && !mascot->looking_right))
        {
            // Turn is unnecessary
            return false;
        }
        else {
            mascot->looking_right = will_look_right;
        }
    }
    return animate::tick();
}

}
}