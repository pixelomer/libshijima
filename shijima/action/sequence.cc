#include "sequence.hpp"

namespace shijima {
namespace action {

bool sequence::requests_interpolation() {
    return false;
}

std::shared_ptr<base> sequence::next_action() {
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
    mascot::tick ctx = { script_ctx, {} };
    action->init(ctx);
    return action;
}

void sequence::init(mascot::tick &ctx) {
    base::init(ctx);
    this->script_ctx = ctx.script;
    action_idx = -1;
    next_action();
}

bool sequence::subtick(int idx) {
    if ((idx == 0) && !base::tick()) {
        return false;
    }
    if (action == nullptr) {
        return false;
    }
    while (action != nullptr && !action->subtick(idx) && (idx == 0) &&
        mascot->queued_behavior.empty())
    {
        next_action();
    }
    return action != nullptr;
}

void sequence::finalize() {
    if (action != nullptr) {
        action->finalize();
        action = nullptr;
    }
    script_ctx = nullptr;
    base::finalize();
}

}
}