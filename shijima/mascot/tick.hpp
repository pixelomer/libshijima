#pragma once

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

#include "state.hpp"
#include <shijima/scripting/context.hpp>
#include <shijima/log.hpp>
#include <memory>

namespace shijima {
namespace mascot {

class tick {
public:
    std::shared_ptr<scripting::context> script;
    std::map<std::string, std::string> extra_attr;
private:
    std::shared_ptr<int> init_count;
    tick(std::shared_ptr<scripting::context> script,
        std::map<std::string, std::string> const& extra_attr,
        std::shared_ptr<int> init_count): script(script),
        extra_attr(extra_attr), init_count(init_count) {}
public:
    void will_init() {
        ++*init_count;
    }
    void reset() {
        *init_count = 0;
    }
    bool reached_init_limit() {
        return *init_count >= 20;
    }
    tick(std::shared_ptr<scripting::context> script,
        std::map<std::string, std::string> const& extra_attr):
        tick(script, extra_attr, std::make_shared<int>()) {}
    tick(): tick(nullptr, {}, std::make_shared<int>()) {}
    
    mascot::tick overlay(std::map<std::string, std::string> const& new_attr) {
        std::map<std::string, std::string> extra_copy = extra_attr;
        for (std::pair<const std::string, std::string> const& pair : new_attr) {
            extra_copy[pair.first] = pair.second;
        }
        return { script, extra_copy, init_count };
    }
};

}
}
