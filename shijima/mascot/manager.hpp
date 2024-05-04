#pragma once
#include "state.hpp"
#include <shijima/behavior/manager.hpp>
#include <shijima/scripting/context.hpp>
#include <shijima/parser.hpp>
#include <memory>

namespace shijima {
namespace mascot {

class manager {
private:
    behavior::manager behaviors;
    std::shared_ptr<behavior::base> behavior;
    std::shared_ptr<action::base> action;
    mascot::state prev_state;
    void next_behavior(std::string const& name = "") {
        if (name != "") {
            behaviors.set_next(name);
        }
        if (action != nullptr) {
            action->finalize();
        }
        behavior = behaviors.next(state);
        action = behavior->action;
        action->init(state, {});
        std::cout << "new behavior: " << behavior->name << std::endl;
    }
public:
    std::shared_ptr<mascot::state> state;
    manager(std::string const& actions_xml, std::string const& behaviors_xml) {
        state = std::make_shared<mascot::state>();
        shijima::parser parser;
        parser.parse(actions_xml, behaviors_xml);
        behaviors.init(parser.behavior_list, "Fall");
    }
    std::string export_state() {
        auto ctx = scripting::context::get();
        ctx->state = state;
        return ctx->eval_string("JSON.stringify(mascot)");
    }
    void tick() {
        state->time++;
        if (behavior == nullptr) {
            // First tick
            prev_state = *state;
            next_behavior();
        }
        if (state->dragging && behavior->name != "Dragged") {
            next_behavior("Dragged");
        }
        else if (!state->dragging && behavior->name == "Dragged") {
            next_behavior("Fall");
        }
        while (true) {
            prev_state = *state;
            if (action->tick()) {
                break;
            }
            if (!state->env.active_ie.top_border().is_on(state->anchor) &&
                !state->env.floor.is_on(state->anchor))
            {
                next_behavior("Fall");
            }
            else {
                next_behavior();
            }
        }
    }
};

}
}