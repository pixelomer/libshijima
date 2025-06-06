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

#include "select.hpp"

namespace shijima {
namespace action {

std::shared_ptr<base> select::next_action() {
    if (did_execute) {
        if (action != nullptr) {
            action->finalize();
        }
        action = nullptr;
        return nullptr;
    }
    auto ret = sequence::next_action(); 
    did_execute = true;
    return ret;
}

void select::init(mascot::tick &ctx) {
    did_execute = false;
    mascot::tick overlay_ctx = ctx.overlay({ { "Loops", "false "} });
    sequence::init(overlay_ctx);
}

}
}