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
    void next_behavior(std::string const& name = "") {
        if (name != "") {
            behaviors.set_next(name);
        }
        if (action != nullptr) {
            std::cout << "(behavior) " << behavior->name << "::finalize()" << std::endl;
            action->finalize();
        }
        behavior = behaviors.next(state);
        std::cout << "(behavior) " << behavior->name << "::init()" << std::endl;
        action = behavior->action;
        action->init(state, {});
    }
    bool action_tick() {
        bool ret = action->tick();
        return ret;
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
            next_behavior();
        }
        if (state->dragging && behavior->name != "Dragged") {
            next_behavior("Dragged");
        }
        else if (!state->dragging && behavior->name == "Dragged") {
            next_behavior("Thrown");
        }
        while (true) {
            if (action_tick()) {
                break;
            }
            if (!state->on_land()) {
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