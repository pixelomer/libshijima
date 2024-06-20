#pragma once
#include "state.hpp"
#include <shijima/behavior/manager.hpp>
#include <shijima/scripting/context.hpp>
#include <shijima/parser.hpp>
#include <shijima/log.hpp>
#include <memory>

namespace shijima {
namespace mascot {

class manager {
private:
    behavior::manager behaviors;
    std::shared_ptr<behavior::base> behavior;
    std::shared_ptr<action::base> action;
    std::map<std::string, std::string> constants;
    void next_behavior(std::string const& name = "") {
        if (name != "") {
            behaviors.set_next(name);
        }
        if (action != nullptr) {
            if (get_log_level() & SHIJIMA_LOG_BEHAVIORS) {
                log("(behavior) " + behavior->name + "::finalize()");
            }
            action->finalize();
        }

        behavior = behaviors.next(state);
        if (behavior == nullptr) {
            // If the next behavior cannot be determined, reset
            // the mascot as defined by Shimeji-EE
            std::cerr << "warning: no next behavior" << std::endl;

            auto &screen = state->env->screen;
            int right = screen.right + screen.width() / 10;
            int left = screen.left + screen.height() / 10;
            double spawnX = (random() % std::max(1, right - left)) + left;
            double spawnY = state->bounds.height + 128;
            state->anchor = { spawnX, spawnY };
            
            behaviors.set_next("Fall");
            behavior = behaviors.next(state);
        }
        if (get_log_level() & SHIJIMA_LOG_BEHAVIORS) {
            log("(behavior) " + behavior->name + "::init()");
        }
        action = behavior->action;
        action->init(*script_ctx, {});
    }
    bool action_tick() {
        bool ret = action->tick();
        return ret;
    }
public:
    struct initializer {
        math::vec2 anchor;
        std::string behavior;
        initializer(math::vec2 anchor = {0,0}, std::string behavior = "Fall"):
            anchor(anchor), behavior(behavior) {}
    };

    std::shared_ptr<scripting::context> script_ctx;
    std::shared_ptr<mascot::state> state;

    manager &operator=(manager const&) = delete;
    manager &operator=(manager&&) = default;
    manager(manager const&) = delete;
    manager(manager&&) = default;

    std::shared_ptr<const behavior::base> active_behavior() {
        return behavior;
    }
    
    manager(std::string const& actions_xml, std::string const& behaviors_xml,
        initializer init = {}, std::shared_ptr<scripting::context> script_ctx = nullptr)
    {
        shijima::parser parser;
        parser.parse(actions_xml, behaviors_xml);
        if (script_ctx == nullptr) {
            script_ctx = std::make_shared<scripting::context>();
        }
        this->script_ctx = script_ctx;
        state = std::make_shared<mascot::state>();
        state->anchor = init.anchor;
        state->constants = parser.constants;
        behaviors = { *script_ctx, parser.behavior_list, init.behavior };
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
            if (state->drag_with_local_cursor) {
                // Force script to use local cursor
                state->dragging = true;
            }
            next_behavior("Thrown");
            state->dragging = false;
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