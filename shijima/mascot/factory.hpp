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
    factory &operator=(factory &&rhs);
    factory(factory const&) = delete;
    factory(factory &&rhs);
    product spawn(std::string const& name, manager::initializer init = {});
    product spawn(mascot::state::breed_request_data const& breed_request);
    void clear();
    void register_template(tmpl const& tmpl);
    void deregister_template(std::string const& name);
    const std::map<std::string, std::shared_ptr<const tmpl>> &get_all_templates() const;
    std::shared_ptr<const tmpl> get_template(std::string const& name) const;
    factory(std::shared_ptr<scripting::context> ctx = nullptr);
};

}
}
