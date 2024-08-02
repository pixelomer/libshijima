#pragma once

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
    struct product {
        std::shared_ptr<const factory::tmpl> tmpl;
        std::unique_ptr<mascot::manager> manager;
    };
private:
    std::map<std::string, std::shared_ptr<const tmpl>> templates;
public:
    std::shared_ptr<scripting::context> script_ctx;
    std::shared_ptr<mascot::environment> env = nullptr;

    factory &operator=(factory const&) = delete;
    factory &operator=(factory&&) = default;
    factory(factory const&) = delete;
    factory(factory&&) = default;
    
    product spawn(std::string const& name, manager::initializer init = {}) {
        product ret;
        auto& tmpl = ret.tmpl = templates.at(name);
        ret.manager = std::make_unique<mascot::manager>(
            tmpl->actions_xml, tmpl->behaviors_xml, init, script_ctx);
        ret.manager->state->env = env;
        return ret;
    }
    product spawn(mascot::state::breed_request_data const& breed_request) {
        return spawn(breed_request.name, breed_request);
    }
    void register_template(tmpl const& tmpl) {
        if (templates.count(tmpl.name) != 0) {
            throw std::logic_error("cannot register same template twice");
        }
        templates[tmpl.name] = std::make_shared<factory::tmpl>(tmpl);
    }
    void deregister_template(std::string const& name) {
        if (templates.count(name) == 0) {
            throw std::logic_error("no such template");
        }
        templates.erase(name);
    }
    const std::map<std::string, std::shared_ptr<const tmpl>> get_all_templates() {
        return templates;
    }
    std::shared_ptr<const tmpl> get_template(std::string const& name) {
        if (templates.count(name) == 0) {
            return nullptr;
        }
        return templates.at(name);
    }
    factory(std::shared_ptr<scripting::context> ctx = nullptr): script_ctx(ctx) {
        if (script_ctx == nullptr) {
            script_ctx = std::make_shared<scripting::context>();
        }
    }
};

}
}