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

#include "list.hpp"
#include <stdexcept>

namespace shijima {
namespace behavior {

std::vector<std::shared_ptr<base>> list::flatten_unconditional() const {
    std::vector<std::shared_ptr<base>> flat;
    for (auto &behavior : children) {
        flat.push_back(behavior);
    }
    for (auto &sub : sublists) {
        auto sub_flat = sub.flatten_unconditional();
        flat.insert(flat.end(), sub_flat.begin(), sub_flat.end());
    }
    return flat;
}

std::vector<std::shared_ptr<base>> list::flatten(scripting::context &ctx) const {
    std::vector<std::shared_ptr<base>> flat;
    if (condition.eval(ctx)) {
        for (auto &behavior : children) {
            if (behavior->condition.eval(ctx)) {
                flat.push_back(behavior);
            }
        }
    }
    for (auto &sub : sublists) {
        auto sub_flat = sub.flatten(ctx);
        flat.insert(flat.end(), sub_flat.begin(), sub_flat.end());
    }
    return flat;
}

std::shared_ptr<base> list::find(std::string const& name, bool throws) const {
    for (auto &child : children) {
        if (child->name == name) {
            return child;
        }
    }
    for (auto &sublist : sublists) {
        auto result = sublist.find(name, false);
        if (result != nullptr) {
            return result;
        }
    }
    if (throws) {
        throw std::logic_error("could not find behavior: " + name);
    }
    return nullptr;
}

list::list(scripting::condition const& cond): condition(cond) {}
list::list(): condition(true) {}
list::list(std::vector<std::shared_ptr<base>> const& children):
    children(children), condition(true) {}

}
}
