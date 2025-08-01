// 
// libshijima - C++ library for shimeji desktop mascots
// Copyright (C) 2024-2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#include "manager.hpp"
#include "shijima/mascot/environment.hpp"
#include <stdexcept>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wcpp"
// prevent backward_warning.h from being included
#ifndef _BACKWARD_BACKWARD_WARNING_H
#define _BACKWARD_BACKWARD_WARNING_H 1
#endif
#include <strstream>
#pragma GCC diagnostic pop

namespace shijima {
namespace mascot {

void manager::_next_behavior(std::string const& name) {
    if (name != "") {
        behaviors.set_next(name);
    }
    if (action != nullptr) {
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BEHAVIORS, "(behavior) " + state->behavior->name
                + "::finalize()");
        #endif
        action->finalize();
    }

    state->behavior = behaviors.next(state);
    if (state->behavior == nullptr) {
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_WARNINGS, "warning: no next behavior");
        #endif

        //reset_position();
        
        behaviors.set_next("Fall");
        state->behavior = behaviors.next(state);
    }
    
    // stop active sound before moving onto next behavior
    state->active_sound.clear();
    state->active_sound_changed = true;

    #ifdef SHIJIMA_LOGGING_ENABLED
        log(SHIJIMA_LOG_BEHAVIORS, "(behavior) " + state->behavior->name
            + "::init()");
    #endif
    action = state->behavior->action;
    action->init(tick_ctx);
}

bool manager::action_tick() {
    // the return value of tick() is ignored for all subticks other than 0
    bool ignore_did_tick = (state->next_subtick != 0);
    bool did_tick = action->subtick(state->next_subtick);
    bool ret = ignore_did_tick || did_tick;
    if (ret) {
        state->next_subtick = (state->next_subtick + 1) % state->env->subtick_count;
    }
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
    state->interaction.finalize();
    state->queued_behavior = name;
}

std::shared_ptr<const behavior::base> manager::active_behavior() {
    return state->behavior;
}

#if !defined(SHIJIMA_NO_PUGIXML)
manager::manager(std::string const& actions_xml, std::string const& behaviors_xml,
    initializer init, std::shared_ptr<scripting::context> script_ctx)
{
    shijima::parser parser;
    parser.parse(actions_xml, behaviors_xml);
    this->init(parser, init, script_ctx);
}
#endif // !defined(SHIJIMA_NO_PUGIXML)

manager::manager(const char *serialized_data, size_t length, initializer init,
    std::shared_ptr<scripting::context> script_ctx)
{
    //FIXME: istrstream is deprecated
    // stringstream performs a copy so it is not ideal
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    std::istrstream stream { serialized_data, (long)length };
#pragma GCC diagnostic pop
    shijima::parser parser;
    parser.loadFrom(stream);
    this->init(parser, init, script_ctx);
}

void manager::init(shijima::parser const& parser, initializer const& init,
    std::shared_ptr<scripting::context> script_ctx)
{
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

bool manager::has_queued_behavior() {
    return state->queued_behavior != "";
}

void manager::activate_queued_behavior() {
    if (has_queued_behavior()) {
        state->next_subtick = 0;
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
}

void manager::pre_tick() {
    state->active_sound_changed = false;
    state->hotspot_triggered = false;
    tick_ctx.reset();
    script_ctx->state = state;
    if (state->env->get_scale() != 1.0) {
        state->local_cursor *= state->env->get_scale();
        state->anchor *= state->env->get_scale();
    }
    state->roll_dcursor();
    state->active_ie_offset.x += state->env->active_ie.dx;
    state->active_ie_offset.y += state->env->active_ie.dy;
    if (state->next_subtick == 0) {
        state->time++;
        if (state->behavior == nullptr) {
            // First tick
            _next_behavior();
        }
        activate_queued_behavior();
        if (state->env->sticky_ie && state->was_on_ie &&
            state->env->floor.y > state->anchor.y)
        {
            // Try to stick to IE by following its dx/dy.
            auto anchor = state->anchor;
            anchor += state->active_ie_offset;
            if (state->env->active_ie.is_on(anchor)) {
                state->anchor = anchor;
            }
        }
        state->active_ie_offset = { 0, 0 };
    }
    else if (state->behavior == nullptr) {
        throw std::runtime_error("cannot determine first behavior on non-zero subtick");
    }
}

void manager::post_tick() {
    if (state->next_subtick == 1 || state->env->subtick_count == 1) {
        // completed subtick 0
        state->was_on_ie = state->env->active_ie.is_on(state->anchor) &&
            !state->env->floor.is_on(state->anchor);
    }
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
        bool did_tick = action_tick();
        if (has_queued_behavior()) {
            activate_queued_behavior();
            continue;
        }
        if (did_tick) {
            break;
        }
        if (tick_ctx.reached_init_limit()) {
            return false;
        }
        state->interaction.finalize();
        /*if (!state->on_land()) {
            _next_behavior("Fall");
        }
        else {
            _next_behavior();
        }*/
        _next_behavior();
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
