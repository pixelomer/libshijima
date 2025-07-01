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
    start_time = mascot->time;
    std::map<std::string, std::string> attr = init_attr;
    for (auto const& pair : ctx.extra_attr) {
        attr[pair.first] = pair.second;
    }
    if (requests_vars()) {
        vars.init(*ctx.script, attr);
        if (requests_broadcast()) {
            auto affordance = vars.get_string("Affordance");
            if (affordance != "") {
                server = mascot->env->broadcasts.start_broadcast(
                    vars.get_string("Affordance"), mascot->anchor);
                vars.add_attr({ { "Affordance", "" } });
            }
        }
    }
}

bool base::tick() {
    if (!requests_vars()) {
        return true;
    }
    if (server.active()) {
        server.update_anchor(mascot->anchor);
    }
    if (server.did_meet_up()) {
        mascot->interaction = server.get_interaction();
        mascot->queued_behavior = mascot->interaction.behavior();
        if (server.turn_requested()) {
            mascot->looking_right = server.requested_looking_right();
        }
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BROADCASTS, "Server did meet client, starting interaction");
            log(SHIJIMA_LOG_BROADCASTS, "Queued behavior: " + mascot->queued_behavior);
        #endif
        return true;
    }
    vars.tick();
    if (!vars.get_bool("Condition", true)) {
        return false;
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
        server.finalize();
        client.finalize();
        vars.finalize();
    }
    mascot = nullptr;
    active = false;
}

}
}