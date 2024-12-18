#include "list.hpp"

namespace shijima {
namespace behavior {

std::vector<std::shared_ptr<base>> list::flatten_unconditional() const {
    std::vector<std::shared_ptr<base>> flat;
    for (auto &behavior : children) {
        flat.push_back(behavior);
    }
    for (auto &sub : sublists) {
        auto sub_flat = sub.flatten_unconditional();
        flat.insert(flat.end(), sub_flat.begin(), sub_flat.end());
    }
    return flat;
}

std::vector<std::shared_ptr<base>> list::flatten(scripting::context &ctx) const {
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

std::shared_ptr<base> list::find(std::string const& name, bool throws) const {
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
        throw std::logic_error("could not find behavior: " + name);
    }
    return nullptr;
}

list::list(scripting::condition const& cond): condition(cond) {}
list::list(): condition(true) {}
list::list(std::vector<std::shared_ptr<base>> const& children):
    children(children), condition(true) {}

}
}