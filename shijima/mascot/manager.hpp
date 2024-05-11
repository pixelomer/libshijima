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
            /*
            std::cout << "(behavior) " << behavior->name << "::finalize()" << std::endl;
            */
            action->finalize();
        }

        behavior = behaviors.next(state);
        if (behavior == nullptr) {
            // If the next behavior cannot be determined, reset
            // the mascot as defined by Shimeji-EE
            std::cerr << "warning: no next behavior" << std::endl;

            auto &screen = state->env.screen;
            int right = screen.right + screen.width() / 10;
            int left = screen.left + screen.height() / 10;
            double spawnX = (random() % std::max(1, right - left)) + left;
            double spawnY = state->bounds.height + 128;
            state->anchor = { spawnX, spawnY };
            
            behaviors.set_next("Fall");
            behavior = behaviors.next(state);
        }
        //std::cout << "(behavior) " << behavior->name << "::init()" << std::endl;
        action = behavior->action;
        action->init(*script_ctx, {});
    }
    bool action_tick() {
        bool ret = action->tick();
        return ret;
    }
public:
    std::shared_ptr<scripting::context> script_ctx;
    std::shared_ptr<mascot::state> state;
    manager(std::string const& actions_xml, std::string const& behaviors_xml,
        std::shared_ptr<scripting::context> script_ctx = nullptr)
    {
        shijima::parser parser;
        parser.parse(actions_xml, behaviors_xml);
        if (script_ctx == nullptr) {
            script_ctx = std::make_shared<scripting::context>();
        }
        this->script_ctx = script_ctx;
        state = std::make_shared<mascot::state>();
        behaviors = { *script_ctx, parser.behavior_list, "Fall" };
    }
    std::string export_state() {
        script_ctx->state = state;
        return script_ctx->eval_string("JSON.stringify(mascot)");
    }
    void tick() {
        script_ctx->state = state;
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