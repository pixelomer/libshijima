#pragma once
#include <memory>
#include "base.hpp"
#include "list.hpp"
#include <shijima/scripting/context.hpp>
#include <shijima/mascot/state.hpp>

namespace shijima {
namespace behavior {

class manager {
public:
    list const& get_initial_list();
private:
    list initial_list;
    list next_list;
    scripting::context::global global;
public:
    manager &operator=(manager const&) = delete;
    manager &operator=(manager&&) = default;
    manager(manager const&) = delete;
    manager(manager&&) = default;
    manager();
    manager(scripting::context &ctx, list initial_list,
        std::string const& first_behavior);
    void set_next(std::string const& next_name);
    std::shared_ptr<base> next(std::shared_ptr<mascot::state> state);
};

}
}