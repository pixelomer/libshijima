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

    std::vector<std::shared_ptr<base>> flatten(scripting::context &ctx) {
        std::vector<std::shared_ptr<base>> flat;
        if (condition.eval(ctx)) {
            for (auto &behavior : children) {
                if (behavior->condition.eval(ctx)) {
                    flat.push_back(behavior);
                }
            }
        }
        for (auto &sub : sublists) {
            auto sub_flat = sub.flatten(ctx);
            flat.insert(flat.end(), sub_flat.begin(), sub_flat.end());
        }
        return flat;
    }

    // Does not follow base::next_list
    std::shared_ptr<base> find(std::string const& name, bool throws = true) {
        for (auto &child : children) {
            if (child->name == name) {
                return child;
            }
        }
        for (auto &sublist : sublists) {
            auto result = sublist.find(name, false);
            if (result != nullptr) {
                return result;
            }
        }
        if (throws) {
            throw std::logic_error("could not find behavior");
        }
        return nullptr;
    }

    list(scripting::condition const& cond): condition(cond) {}
    list(): condition(true) {}
    list(std::vector<std::shared_ptr<base>> const& children):
        children(children), condition(true) {}
};

}
}