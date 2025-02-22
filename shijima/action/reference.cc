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

#include "reference.hpp"

namespace shijima {
namespace action {

bool reference::requests_vars() {
    return false;
}

bool reference::requests_interpolation() {
    return false;
}

void reference::init(mascot::tick &ctx) {
    base::init(ctx);
    mascot::tick target_ctx = ctx.overlay(init_attr);
    try {
        target->init(target_ctx);
    }
    catch (...) {
        base::finalize();
        throw;
    }
}

bool reference::subtick(int idx) {
    if (!base::subtick(idx)) {
        return false;
    }
    return target->subtick(idx);
}

void reference::finalize() {
    base::finalize();
    target->finalize();
}

}
}