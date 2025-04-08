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
    void init(std::string const& str) {
        if (str.size() > 3 && (str[0] == '$' || str[0] == '#') &&
            str[1] == '{' && str[str.size()-1] == '}')
        {
            is_constant = false;
            js = str.substr(2, str.size()-3);
        }
        else {
            is_constant = true;
            value = (str == "true");
        }
    }
    void init(bool val) {
        this->is_constant = true;
        this->value = val;
    }
public:
    condition(std::string const& str) {
        init(str);
    }
    condition(const char *str) {
        init(std::string(str));
    }
    condition(bool value) {
        init(value);
    }
    bool eval(context &ctx) const {
        if (is_constant) {
            return value;
        }
        return ctx.eval_bool(js);
    }
    bool eval(context::global::active &ctx) const {
        if (is_constant) {
            return value;
        }
        return ctx->eval_bool(js);
    }
    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(is_constant, value, js);
    }
};

}
}
