#pragma once
#include <vector>
#include <memory>
#include <shijima/action/base.hpp>
#include <shijima/scripting/context.hpp>
#include <shijima/scripting/condition.hpp>

namespace shijima {
namespace behavior {

class list;

class base {
public:
    // From <NextBehaviorList>
    std::unique_ptr<list> next_list; //FIXME: unique_ptr is a hack to fix circular dependency. This should be redesigned.
    bool add_next = true;

    // From <Behavior>
    std::string name;
    int frequency;
    bool hidden;
    scripting::condition condition;

    // For <BehaviorReference>
    std::shared_ptr<base> referenced;

    // Referred action
    std::shared_ptr<action::base> action;

    base(std::string const& name, int freq, bool hidden,
        scripting::condition const& cond): name(name), frequency(freq),
        hidden(hidden), condition(cond) {}
};

}
}