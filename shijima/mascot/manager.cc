#include "manager.hpp"

namespace shijima {
namespace mascot {

void manager::_next_behavior(std::string const& name) {
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

bool manager::action_tick() {
    bool ret = action->tick();
    return ret;
}

void manager::reset_position() {
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

void manager::detach_from_borders() {
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

void manager::next_behavior(std::string const& name) {
    // Only for public use
    tick_ctx.reset();
    _next_behavior(name);
}

std::shared_ptr<const behavior::base> manager::active_behavior() {
    return behavior;
}

manager::manager(std::string const& actions_xml, std::string const& behaviors_xml,
    initializer init, std::shared_ptr<scripting::context> script_ctx)
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

std::string manager::export_state() {
    script_ctx->state = state;
    return script_ctx->eval_string("JSON.stringify(mascot)");
}

void manager::pre_tick() {
    tick_ctx.reset();
    script_ctx->state = state;
    state->time++;
    state->active_sound_changed = false;
    if (state->env->get_scale() != 1.0) {
        state->local_cursor *= state->env->get_scale();
        state->anchor *= state->env->get_scale();
    }
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
        _next_behavior("Thrown");
        state->was_on_ie = false;
        state->dragging = false;
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

void manager::post_tick() {
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
}

bool manager::_tick() {
    while (true) {
        if (action_tick()) {
            break;
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

void manager::tick() {
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
    throw std::logic_error("tick() failed after multiple attempts");
}

}
}