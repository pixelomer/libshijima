#pragma once
#include "base.hpp"
#include <vector>
#include <memory>

namespace shijima {
namespace action {

class sequence : public base {
protected:
    int action_idx = -1;
    std::shared_ptr<base> action;
    virtual std::shared_ptr<base> next_action() {
        if (action_idx >= (int)actions.size()) {
            if (vars.get_bool("Loops", false)) {
                action_idx = -1;
            }
            else {
                return nullptr;
            }
        }
        if (action != nullptr) {
            action->finalize();
            action = nullptr;
        }
        action_idx++;
        if (action_idx >= (int)actions.size()) {
            if (vars.get_bool("Loops", false)) {
                action_idx = 0;
            }
            else {
                return nullptr;
            }
        }
        action = actions[action_idx];
        action->init(mascot, {});
        return action;
    }
public:
    std::vector<std::shared_ptr<base>> actions;
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra)
    {
        base::init(mascot, extra);
        action_idx = -1;
        next_action();
    }
    virtual bool tick() {
        if (!base::tick()) {
            return false;
        }
        if (action == nullptr) {
            return false;
        }
        while (action != nullptr && !action->tick()) {
            next_action();
        }
        return action != nullptr;
    }
    virtual void finalize() {
        if (action != nullptr) {
            action->finalize();
            action = nullptr;
        }
        base::finalize();
    }
};

}
}