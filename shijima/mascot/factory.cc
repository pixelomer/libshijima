#include "factory.hpp"
#include <stdexcept>

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
    ret.manager = std::make_unique<mascot::manager>(
        tmpl->actions_xml, tmpl->behaviors_xml, init, script_ctx);
    ret.manager->state->env = env;
    return ret;
}

factory::product factory::spawn(mascot::state::breed_request_data const& breed_request) {
    return spawn(breed_request.name, breed_request);
}

void factory::clear() {
    templates.clear();
}

void factory::register_template(tmpl const& tmpl) {
    if (templates.count(tmpl.name) != 0) {
        throw std::logic_error("cannot register same template twice");
    }
    templates[tmpl.name] = std::make_shared<factory::tmpl>(tmpl);
}

void factory::deregister_template(std::string const& name) {
    if (templates.count(name) == 0) {
        throw std::logic_error("no such template");
    }
    templates.erase(name);
}

const std::map<std::string, std::shared_ptr<const factory::tmpl>> &factory::get_all_templates() const {
    return templates;
}

std::shared_ptr<const factory::tmpl> factory::get_template(std::string const& name) const {
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