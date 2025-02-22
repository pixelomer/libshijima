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

#include <map>
#include <set>
#include "condition.hpp"
#include "context.hpp"

namespace shijima {
namespace scripting {

class variables {
private:
    std::map<std::string, std::string> dynamic_attr;
    std::set<std::string> attr_keys;
    scripting::context::global global;
    static signed char dynamic_prefix(std::string const& str);
    static bool is_num(std::string const& str);
public:
    variables();
    void add_attr(std::map<std::string, double> const& attr);
    void add_attr(std::map<std::string, std::string> const& attr);
    void init(scripting::context &ctx,
        std::map<std::string, std::string> const& attr);
    std::string dump();
    void tick();
    void finalize();
    double get_num(std::string const& key, double fallback = 0.0);
    bool get_bool(std::string const& key, bool fallback = false);
    std::string get_string(std::string const& key, std::string const& fallback = "");
    bool get_bool(scripting::condition &cond);
    bool has(std::string const& key);
};

}
}
