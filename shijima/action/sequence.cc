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

#include "sequence.hpp"

namespace shijima {
namespace action {

bool sequence::requests_interpolation() {
    return false;
}

void sequence::next_action() {
    if (action != nullptr) {
        action->finalize();
        action = nullptr;
    }
    if (action_idx >= (int)actions.size()) {
        if (vars.get_bool("Loop", false)) {
            action_idx = -1;
        }
        else {
            return;
        }
    }
    action_idx++;
    if (action_idx >= (int)actions.size()) {
        if (vars.get_bool("Loop", false)) {
            action_idx = 0;
        }
        else {
            return;
        }
    }
    action = actions.at(action_idx);
    mascot::tick ctx = { script_ctx, {} };
    action->init(ctx);
}

void sequence::init(mascot::tick &ctx) {
    base::init(ctx);
    this->script_ctx = ctx.script;
    action_idx = -1;
    next_action();
}

bool sequence::subtick(int idx) {
    if ((idx == 0) && !base::tick()) {
        return false;
    }
    if (action == nullptr) {
        return false;
    }
    size_t attempts = 0;
    size_t attempts_max = actions.size() + 1;
    while (action != nullptr && !action->subtick(idx) && (idx == 0) &&
        mascot->queued_behavior.empty() && (attempts != attempts_max))
    {
        next_action();
        ++attempts;
    }
    if (attempts == attempts_max && action != nullptr) {
        action->finalize();
        action = nullptr;
    }
    return action != nullptr;
}

void sequence::finalize() {
    if (action != nullptr) {
        action->finalize();
        action = nullptr;
    }
    script_ctx = nullptr;
    base::finalize();
}

}
}