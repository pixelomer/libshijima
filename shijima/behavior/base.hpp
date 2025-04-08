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

#include "../config.hpp"
#include <memory>
#include <shijima/action/base.hpp>
#include <shijima/scripting/condition.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
//#include <cereal/types/polymorphic.hpp>

namespace shijima {
namespace behavior {

class list;

class base {
public:
    // From <NextBehaviorList>
    std::unique_ptr<list> next_list; //FIXME: unique_ptr is a hack to fix circular dependency. This should be redesigned.
    bool add_next = true;

    // From <Behavior>
    std::string name;
    int frequency;
    bool hidden;
    scripting::condition condition;

    // For <BehaviorReference>
    std::shared_ptr<base> referenced;

    // Referred action
    std::shared_ptr<action::base> action;

    explicit base();
    base(std::string const& name, int freq, bool hidden,
        scripting::condition const& cond);
    
    template<class Archive>
    void serialize(Archive &ar) {
        ar(next_list, add_next, name, frequency, hidden, condition, referenced,
            action);
    }
};

}
}
