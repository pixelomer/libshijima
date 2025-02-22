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

#include "manager.hpp"

namespace shijima {
namespace behavior {

list const& manager::get_initial_list() {
    return initial_list;
}

manager::manager(): initial_list(), next_list() {}

manager::manager(scripting::context &ctx, list initial_list,
    std::string const& first_behavior): initial_list(initial_list), next_list()
{
    global = ctx.make_global();
    set_next(first_behavior);
}

void manager::set_next(std::string const& next_name) {
    next_list = {};
    if (!next_name.empty()) {
        next_list.children.push_back(initial_list.find(next_name));
    }
    else {
        next_list.sublists.push_back(initial_list);
    }
}

std::shared_ptr<base> manager::next(std::shared_ptr<mascot::state> state) {
    auto ctx = global.use();
    ctx->state = state;
    auto flat = next_list.flatten(*ctx.get());

    std::shared_ptr<base> behavior;
    
    if (flat.size() == 1) {
        // Use the only option
        behavior = flat[0];
    }
    else {
        // Sum all frequencies
        int freq_sum = 0;
        for (auto &option : flat) {
            freq_sum += option->frequency;
        }
        if (freq_sum == 0) {
            return nullptr;
        }

        // Pick a random behavior
        int counter = 0;
        int dice = ctx->random(freq_sum);
        for (auto &option : flat) {
            counter += option->frequency;
            if (counter > dice) {
                behavior = option;
                break;
            }
        }
    }

    // Behavior may be a reference, use the original
    if (behavior->referenced != nullptr) {
        behavior = behavior->referenced;
    }

    // Adjust next_list
    if (!behavior->add_next) {
        // If "Add"="false", only use the options provided by the behavior
        next_list = *behavior->next_list;
    }
    else {
        // Otherwise, merge the initial list and the behavior list
        next_list = {};
        next_list.sublists.push_back(initial_list);
        if (behavior->next_list != nullptr) {
            next_list.sublists.push_back(*behavior->next_list);
        }
    }

    return behavior;
}

}
}
