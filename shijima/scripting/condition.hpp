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

#include "context.hpp"

namespace shijima {
namespace scripting {

class condition {
private:
    bool is_constant;
    bool value;
    std::string js;
    void init(std::string const& str);
    void init(bool val);
public:
    condition(std::string const& str);
    condition(const char *str);
    condition(bool value);
    bool eval(context &ctx) const;
    bool eval(context::global::active &ctx) const;
    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(is_constant, value, js);
    }
};

}
}
