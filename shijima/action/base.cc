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

#include "base.hpp"
#include <stdexcept>

namespace shijima {
namespace action {

bool base::requests_vars() {
    return true;
}

bool base::requests_broadcast() {
    return false;
}

bool base::requests_interpolation() {
    return true;
}

bool base::requests_periodic_breed() {
    return false;
}

void base::init(mascot::tick &ctx) {
    ctx.will_init();
    #ifdef SHIJIMA_LOGGING_ENABLED
        if (get_log_level() & SHIJIMA_LOG_ACTIONS) {
            if (init_attr.count("Name") == 1) {
                log("(action) " + init_attr.at("Name") + "::init()");
            }
            else {
                log("(action) <type:" + init_attr.at("Type") + ">::init()");
            }
        }
    #endif
    if (active) {
        throw std::logic_error("init() called twice");
    }
    active = true;
    mascot = ctx.script->state;
    real_start_time = start_time = mascot->time;
    std::map<std::string, std::string> attr = init_attr;
    for (auto const& pair : ctx.extra_attr) {
        attr[pair.first] = pair.second;
    }
    if (requests_vars()) {
        vars.init(*ctx.script, attr);
        if (requests_broadcast()) {
            auto affordance = vars.get_string("Affordance");
            if (affordance != "") {
                if (mascot->server.active() && mascot->server.affordance() != affordance) {
                    mascot->server.finalize();
                }
                if (!mascot->server.active()) {
                    mascot->server = mascot->env->broadcasts.start_broadcast(
                        affordance, mascot->anchor);
                }
            }
            else if (mascot->server.active()) {
                mascot->server.finalize();
            }
        }
        else if (mascot->server.active()) {
            mascot->server.finalize();
        }
    }
}

bool base::tick() {
    if (!requests_vars()) {
        return true;
    }
    if (mascot->server.active()) {
        mascot->server.update_anchor(mascot->anchor);
    }
    vars.tick();
    if (mascot->server.did_meet_up()) {
        mascot->interaction = mascot->server.get_interaction();
        mascot->queued_behavior = mascot->interaction.behavior();
        if (mascot->server.turn_requested()) {
            mascot->looking_right = mascot->server.requested_looking_right();
        }
        if (mascot->queued_behavior.empty()) {
            // TargetBehavior was not set, an interaction cannot happen
            mascot->interaction.finalize();

            // Reset the server so that other clients may find it
            mascot->server.finalize();
            mascot->server = mascot->env->broadcasts.start_broadcast(
                vars.get_string("Affordance"), mascot->anchor);
        }
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BROADCASTS, "Server did meet client");
            if (!mascot->queued_behavior.empty()) {
                log(SHIJIMA_LOG_BROADCASTS, "Starting interaction with behavior: "
                    + mascot->queued_behavior);
            }
        #endif
        if (!mascot->queued_behavior.empty()) {
            mascot->server.finalize();
            return true;
        }
    }
    if (!vars.get_bool("Condition", true)) {
        return false;
    }
    if (vars.has("Duration") && real_elapsed() >= (int)vars.get_num("Duration", INT_MAX)) {
        return false;
    }
    if (requests_periodic_breed()) {
        int interval = (int)vars.get_num("BornInterval");
        if (interval > 0 && real_elapsed() % interval == 0) {
            bool transient = vars.get_bool("BornTransient", false);
            if (!transient && (!mascot->env->allows_breeding || !mascot->can_breed)) {
                // Not allowed to breed, but action can continue
                return true;
            }

            // Create breed request and continue
            auto &request = mascot->breed_request;
            double born_x = vars.get_num("BornX", 0);
            double born_y = vars.get_num("BornY", 0);
            request.available = true;
            request.behavior = vars.get_string("BornBehavior", "Fall");
            request.name = vars.get_string("BornMascot", "");
            request.transient = transient;
            
            request.anchor = { mascot->anchor.x + dx(born_x),
                mascot->anchor.y + dy(born_y) };
        }
    }
    return true;
}

bool base::subtick(int idx) {
    if (requests_interpolation()) {
        if (idx == 0) {
            auto start_anchor = mascot->anchor;
            if (!tick()) {
                return false;
            }
            target_offset = mascot->anchor - start_anchor;
            mascot->anchor = start_anchor;
        }
        mascot->anchor += target_offset *
            (1 / (double)mascot->env->subtick_count);
        return true;
    }
    else {
        return (idx != 0) || tick();
    }
}

void base::finalize() {
    #ifdef SHIJIMA_LOGGING_ENABLED
        if (get_log_level() & SHIJIMA_LOG_ACTIONS) {
            if (init_attr.count("Name") == 1) {
                log("(action) " + init_attr.at("Name") + "::finalize()");
            }
            else {
                log("(action) <type:" + init_attr.at("Type") + ">::finalize()");
            }
        }
    #endif
    if (!active) {
        throw std::runtime_error("finalize() called twice");
    }
    if (mascot->next_subtick != 0) {
        throw std::runtime_error("finalize() called at non-zero subtick");
    }
    if (requests_vars()) {
        mascot->client.finalize();
        vars.finalize();
    }
    mascot = nullptr;
    active = false;
}

}
}