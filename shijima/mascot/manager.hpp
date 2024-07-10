#pragma once
#include "state.hpp"
#include "tick.hpp"
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
    mascot::tick tick_ctx;
    std::shared_ptr<behavior::base> behavior;
    std::shared_ptr<action::base> action;
    std::map<std::string, std::string> constants;
private:
    void _next_behavior(std::string const& name = "") {
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

            reset_position();
            
            behaviors.set_next("Fall");
            behavior = behaviors.next(state);
        }
        if (get_log_level() & SHIJIMA_LOG_BEHAVIORS) {
            log("(behavior) " + behavior->name + "::init()");
        }
        action = behavior->action;
        action->init(tick_ctx);
    }
    bool action_tick() {
        bool ret = action->tick();
        return ret;
    }
public:
    void reset_position() {
        auto &screen = state->env->screen;
        if (screen.width() >= 100 && screen.height() >= 100) {
            double new_x = static_cast<double>(screen.left + 50 +
                random() % (screen.width() - 50));
            double new_y = static_cast<double>(screen.top + 50 +
                random() % (screen.height() - 50));
            state->anchor = { new_x, new_y };
        }
        else {
            double new_x = static_cast<double>(screen.width() / 2);
            double new_y = static_cast<double>(screen.height() / 2);
            state->anchor = { new_x, new_y };
        }
    }
    void next_behavior(std::string const& name = "") {
        // Only for public use
        tick_ctx.reset();
        _next_behavior(name);
    }

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
        tick_ctx.script = script_ctx;
        state = std::make_shared<mascot::state>();
        state->anchor = init.anchor;
        state->constants = parser.constants;
        behaviors = { *script_ctx, parser.behavior_list, init.behavior };
    }
    std::string export_state() {
        script_ctx->state = state;
        return script_ctx->eval_string("JSON.stringify(mascot)");
    }
private:
    bool _tick() {
        while (true) {
            if (action_tick()) {
                return true;
            }
            if (tick_ctx.reached_init_limit()) {
                return false;
            }
            if (!state->on_land()) {
                _next_behavior("Fall");
            }
            else {
                _next_behavior();
            }
        }
    }
public:
    void tick() {
        tick_ctx.reset();
        script_ctx->state = state;
        state->time++;
        if (behavior == nullptr) {
            // First tick
            _next_behavior();
        }
        if (state->dragging && behavior->name != "Dragged") {
            _next_behavior("Dragged");
        }
        else if (!state->dragging && behavior->name == "Dragged") {
            if (state->drag_with_local_cursor) {
                // Force script to use local cursor
                state->dragging = true;
            }
            _next_behavior("Thrown");
            state->dragging = false;
        }

        // Attempt 1: Normal tick
        if (_tick()) return;

        // Attempt 2: Set behavior to Fall, try again
        if (get_log_level() & SHIJIMA_LOG_WARNINGS) {
            log("warning: init() limit reached, trying \"Fall\" behavior");
        }
        tick_ctx.reset();
        _next_behavior("Fall");
        if (_tick()) return;

        // Attempt 3: Set behavior to Fall, reset position, try again
        if (get_log_level() & SHIJIMA_LOG_WARNINGS) {
            log("warning: init() limit reached again, will reset position "
                "and try again");
        }
        tick_ctx.reset();
        reset_position();
        _next_behavior("Fall");
        if (_tick()) return;

        // If this point in the code is reached, then the mascot is
        // most likely faulty.
        throw std::logic_error("tick() failed after multiple attempts");
    }
};

}
}