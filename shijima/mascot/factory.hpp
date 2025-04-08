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

#include <string>
#include <map>
#include "manager.hpp"
#include "state.hpp"

namespace shijima {
namespace mascot {

class factory {
public:
    struct tmpl {
        std::string name;
        std::string actions_xml;
        std::string behaviors_xml;
        std::string path;
    };
    struct registered_tmpl {
        std::string name;
        std::string data; // serialized binary data
        std::string path;
        registered_tmpl() {}
        registered_tmpl(std::string const& name, std::string const& data,
            std::string const& path): name(name), data(data), path(path) {}
    };
    struct product {
        std::shared_ptr<const factory::registered_tmpl> tmpl;
        std::unique_ptr<mascot::manager> manager;
    };
private:
    std::map<std::string, std::shared_ptr<const registered_tmpl>> templates;
public:
    std::shared_ptr<scripting::context> script_ctx;
    std::shared_ptr<mascot::environment> env = nullptr;

    factory &operator=(factory const&) = delete;
    factory &operator=(factory &&rhs);
    factory(factory const&) = delete;
    factory(factory &&rhs);
    product spawn(std::string const& name, manager::initializer init = {});
    product spawn(mascot::state::breed_request_data const& breed_request);
    void clear();
    void register_template(tmpl const& tmpl);
    void deregister_template(std::string const& name);
    const std::map<std::string, std::shared_ptr<const registered_tmpl>> &get_all_templates() const;
    std::shared_ptr<const registered_tmpl> get_template(std::string const& name) const;
    factory(std::shared_ptr<scripting::context> ctx = nullptr);
};

}
}
