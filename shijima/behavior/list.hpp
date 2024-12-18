#pragma once
#include "base.hpp"
#include <vector>
#include <memory>
#include <shijima/scripting/context.hpp>
#include <shijima/scripting/condition.hpp>

namespace shijima {
namespace behavior {

class list {
public:
    std::vector<std::shared_ptr<base>> children;
    scripting::condition condition;
    std::vector<list> sublists;

    std::vector<std::shared_ptr<base>> flatten_unconditional() const;

    std::vector<std::shared_ptr<base>> flatten(scripting::context &ctx) const;

    // Does not follow base::next_list
    std::shared_ptr<base> find(std::string const& name, bool throws = true) const;

    list(scripting::condition const& cond);
    list();
    list(std::vector<std::shared_ptr<base>> const& children);
};

}
}