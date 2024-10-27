#pragma once
#include "state.hpp"
#include "tick.hpp"
#include <shijima/behavior/manager.hpp>
#include <shijima/scripting/context.hpp>
#include <shijima/parser.hpp>
#include <shijima/log.hpp>
#include <memory>
#include <random>

namespace shijima {
namespace mascot {

class manager {
private:
    behavior::manager behaviors;
    mascot::tick tick_ctx;
    std::shared_ptr<behavior::base> behavior;
    std::shared_ptr<action::base> action;
    std::map<std::string, std::string> constants;
public:
    behavior::list const& initial_behavior_list() {
        return behaviors.get_initial_list();
    }
private:
    void _next_behavior(std::string const& name = "") {
        if (name != "") {
            behaviors.set_next(name);
        }
        if (action != nullptr) {
            #ifdef SHIJIMA_LOGGING_ENABLED
                log(SHIJIMA_LOG_BEHAVIORS, "(behavior) " + behavior->name
                    + "::finalize()");
            #endif
            action->finalize();
        }

        behavior = behaviors.next(state);
        if (behavior == nullptr) {
            #ifdef SHIJIMA_LOGGING_ENABLED
                log(SHIJIMA_LOG_WARNINGS, "warning: no next behavior");
            #endif

            //reset_position();
            
            behaviors.set_next("Fall");
            behavior = behaviors.next(state);
        }
        
        // stop active sound before moving onto next behavior
        state->active_sound.clear();
        state->active_sound_changed = true;

        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BEHAVIORS, "(behavior) " + behavior->name
                + "::init()");
        #endif
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
            double new_x = screen.left + 50 + state->env->random((int)screen.width() - 50);
            double new_y = screen.top + 50 + state->env->random((int)screen.width() - 50);
            state->anchor = { new_x, new_y };
        }
        else {
            double new_x = screen.width() / 2;
            double new_y = screen.height() / 2;
            state->anchor = { new_x, new_y };
        }
    }

    void detach_from_borders() {
        auto &active_ie = state->env->active_ie;
        auto &work_area = state->env->work_area;
        auto &anchor = state->anchor;
        if (active_ie.right_border().is_on(anchor) ||
            work_area.left_border().is_on(anchor))
        {
            anchor.x += 1;
        }
        else if (active_ie.left_border().is_on(anchor) ||
            work_area.right_border().is_on(anchor))
        {
            anchor.x -= 1;
        }
        if (active_ie.bottom_border().is_on(anchor) ||
            work_area.top_border().is_on(anchor))
        {
            anchor.y += 1;
        }
        else if (active_ie.top_border().is_on(anchor) ||
            work_area.bottom_border().is_on(anchor))
        {
            anchor.y -= 1;
        }
    }

    void next_behavior(std::string const& name = "") {
        // Only for public use
        tick_ctx.reset();
        state->queued_behavior = name;
    }

    struct initializer {
        math::vec2 anchor;
        std::string behavior;
        bool looking_right;
        initializer(math::vec2 anchor = {0,0}, std::string behavior = "Fall",
            bool looking_right = false):
            anchor(anchor), behavior(behavior), looking_right(looking_right) {}
        initializer(mascot::state::breed_request_data const& data):
            anchor(data.anchor), behavior(data.behavior),
            looking_right(data.looking_right) {}
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
        state->looking_right = init.looking_right;
        behaviors = { *script_ctx, parser.behavior_list, init.behavior };
    }
    std::string export_state() {
        script_ctx->state = state;
        return script_ctx->eval_string("JSON.stringify(mascot)");
    }
private:
    void pre_tick() {
        tick_ctx.reset();
        script_ctx->state = state;
        auto subticks_per_tick = state->env->subticks_per_tick;
        if (behavior == nullptr) {
            // First tick
            state->time = 0;
            state->subtick = 0;
        }
        else {
            state->subtick = (state->subtick + 1) % subticks_per_tick;
            if (state->subtick == 0) {
                ++state->time;
            }
        }
        state->active_sound_changed = false;
        if (state->env->get_scale() != 1.0) {
            state->local_cursor *= state->env->get_scale();
            state->anchor *= state->env->get_scale();
        }
        if (subticks_per_tick != 1) {
            state->local_cursor.dx /= subticks_per_tick;
            state->local_cursor.dy /= subticks_per_tick;
            state->env->cursor.dx /= subticks_per_tick;
            state->env->cursor.dy /= subticks_per_tick;
        }
        if (state->new_tick()) {
            // Certain things should only be updated after a full tick
            if (behavior == nullptr) {
                // First tick
                _next_behavior();
            }
            if (state->queued_behavior != "") {
                auto &behavior = state->queued_behavior;
                if (state->interaction.available() && !state->interaction.started &&
                    state->interaction.behavior() == behavior)
                {
                    // Start ScanMove/Broadcast interaction
                    state->interaction.started = true;
                }
                _next_behavior(behavior);
                behavior = "";
            }
            bool dragging;
            if (state->drag_lock > 0) {
                dragging = false;
            }
            else {
                dragging = state->dragging;
            }
            if (dragging && behavior->name != "Dragged") {
                state->was_on_ie = false;
                state->interaction.finalize();
                _next_behavior("Dragged");
            }
            else if (!dragging && behavior->name == "Dragged") {
                if (state->drag_with_local_cursor) {
                    // Force script to use local cursor
                    state->dragging = true;
                }
                state->interaction.finalize();
                auto dcursor = state->get_dcursor();
                state->get_cursor().dx = dcursor.x;
                state->get_cursor().dy = dcursor.y;
                state->reset_dcursor_buffer();
                _next_behavior("Thrown");
                state->was_on_ie = false;
                state->dragging = false;
            }
        }
        if (state->env->sticky_ie && state->was_on_ie &&
            state->env->floor.y > state->anchor.y)
        {
            // Try to stick to IE by following its dx/dy.
            auto anchor = state->anchor;
            anchor.x += state->env->active_ie.dx;
            anchor.y += state->env->active_ie.dy;
            if (state->env->active_ie.is_on(anchor)) {
                state->anchor = anchor;
            }
        }
    }
    void post_tick() {
        state->was_on_ie = state->env->active_ie.is_on(state->anchor) &&
            !state->env->floor.is_on(state->anchor);
        if (!state->active_frame.sound.empty() && state->active_sound != state->active_frame.sound) {
            state->active_sound_changed = true;
            state->active_sound = state->active_frame.sound;
        }
        if (state->env->get_scale() != 1.0) {
            state->local_cursor /= state->env->get_scale();
            state->anchor /= state->env->get_scale();
        }
        auto subticks_per_tick = state->env->subticks_per_tick;
        if (subticks_per_tick != 1) {
            state->local_cursor.dx *= subticks_per_tick;
            state->local_cursor.dy *= subticks_per_tick;
            state->env->cursor.dx *= subticks_per_tick;
            state->env->cursor.dy *= subticks_per_tick;
        }
    }
    bool _tick() {
        while (true) {
            if (action_tick()) {
                break;
            }
            if (!state->new_tick()) {
                throw std::runtime_error("tick() failed for subtick != 0");
            }
            if (tick_ctx.reached_init_limit()) {
                return false;
            }
            state->interaction.finalize();
            if (!state->on_land()) {
                _next_behavior("Fall");
            }
            else {
                _next_behavior();
            }
        }
        post_tick();
        return true;
    }
public:
    void tick() {
        if (state->dead) {
            return;
        }
        
        pre_tick();

        // Attempt 1: Normal tick
        if (_tick()) return;

        // Attempt 2: Set behavior to Fall, try again
        tick_ctx.reset();
        _next_behavior("Fall");
        if (_tick()) return;

        // Attempt 3: Set behavior to Fall, detach from borders, try again
        tick_ctx.reset();
        detach_from_borders();
        _next_behavior("Fall");
        if (_tick()) return;

        // Attempt 4: Set behavior to Fall, reset position,
        //            detach from borders, try again
        tick_ctx.reset();
        reset_position();
        detach_from_borders();
        _next_behavior("Fall");
        if (_tick()) return;

        // If this point in the code is reached, then the mascot is
        // most likely faulty.
        throw std::runtime_error("tick() failed after multiple attempts");
    }
};

}
}