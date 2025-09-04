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

#include "factory.hpp"
#include <stdexcept>
#if !defined(SHIJIMA_NO_PUGIXML)
#include <sstream>
#endif

namespace shijima {
namespace mascot {

factory &factory::operator=(factory &&rhs) {
    templates = rhs.templates;
    script_ctx = rhs.script_ctx;
    env = rhs.env;
    rhs.templates.clear();
    rhs.script_ctx = nullptr;
    rhs.env = nullptr;
    return *this;
}

factory::factory(factory &&rhs) {
    *this = std::move(rhs);
}

factory::product factory::spawn(std::string const& name, manager::initializer init) {
    product ret;
    auto& tmpl = ret.tmpl = templates.at(name);
    ret.manager = std::make_unique<mascot::manager>(tmpl->data.c_str(),
        tmpl->data.size(), init, script_ctx);
    ret.manager->get_state()->env = env;
    return ret;
}

factory::product factory::spawn(mascot::state::breed_request_data const& breed_request) {
    return spawn(breed_request.name, breed_request);
}

void factory::clear() {
    templates.clear();
}

#if !defined(SHIJIMA_NO_PUGIXML)

std::shared_ptr<const factory::registered_tmpl> factory::register_template(
    tmpl const& tmpl)
{
    shijima::parser parser;
    return register_template(tmpl, parser);
}

std::shared_ptr<const factory::registered_tmpl> factory::register_template(
    tmpl const& tmpl, shijima::parser &parser)
{
    if (templates.count(tmpl.name) != 0) {
        throw std::logic_error("cannot register same template twice");
    }
    std::ostringstream out;
    parser.parse(tmpl.actions_xml, tmpl.behaviors_xml);
    parser.saveTo(out);
    return templates[tmpl.name] = std::make_shared<factory::registered_tmpl>(
        tmpl.name, out.str(), tmpl.path);
}

#endif

std::shared_ptr<const factory::registered_tmpl> factory::register_template(
    registered_tmpl const& tmpl)
{
    if (templates.count(tmpl.name) != 0) {
        throw std::logic_error("cannot register same template twice");
    }
    return templates[tmpl.name] = std::make_shared<factory::registered_tmpl>(tmpl);
}

void factory::deregister_template(std::string const& name) {
    if (templates.count(name) == 0) {
        throw std::logic_error("no such template");
    }
    templates.erase(name);
}

const std::map<std::string, std::shared_ptr<const factory::registered_tmpl>> &
    factory::get_all_templates() const
{
    return templates;
}

std::shared_ptr<const factory::registered_tmpl> factory::get_template(
    std::string const& name) const
{
    if (templates.count(name) == 0) {
        return nullptr;
    }
    return templates.at(name);
}

factory::factory(std::shared_ptr<scripting::context> ctx): script_ctx(ctx) {
    if (script_ctx == nullptr) {
        script_ctx = std::make_shared<scripting::context>();
    }
}

}
}
