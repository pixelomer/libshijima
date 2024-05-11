#pragma once
#include "base.hpp"
#include <vector>
#include <memory>

namespace shijima {
namespace action {

class sequence : public base {
private:
    //FIXME: Ideally this shouldn't be necessary
    scripting::context *script_ctx;
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
        action->init(*script_ctx, {});
        return action;
    }
public:
    std::vector<std::shared_ptr<base>> actions;
    virtual void init(scripting::context &script_ctx,
        std::map<std::string, std::string> const& extra) override
    {
        base::init(script_ctx, extra);
        this->script_ctx = &script_ctx;
        action_idx = -1;
        next_action();
    }
    virtual bool tick() override {
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
    virtual void finalize() override {
        if (action != nullptr) {
            action->finalize();
            action = nullptr;
        }
        base::finalize();
    }
};

}
}