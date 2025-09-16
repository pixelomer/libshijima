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

#include "scaninteract.hpp"

namespace shijima {
namespace action {

bool scaninteract::requests_broadcast() {
    return false;
}

void scaninteract::init(mascot::tick &ctx) {
    animate::init(ctx);
    mascot->env->broadcasts.try_connect(mascot->client, NAN,
        vars.get_string("Affordance"), vars.get_string("Behavior"),
        vars.get_string("TargetBehavior"));
}

bool scaninteract::tick() {
    if (!mascot->client.connected()) {
        return false;
    }
    auto target = mascot->client.get_target();
    mascot->looking_right = target.x > mascot->anchor.x;
    vars.add_attr({ { "TargetX", target.x }, { "TargetY", target.y } });
    bool ret = animate::tick();
    if (!ret && animation_finished()) {
        if (vars.get_bool("TargetLook", false)) {
            mascot->client.request_turn(!mascot->looking_right);
        }
        mascot->client.notify_arrival();
        mascot->interaction = mascot->client.get_interaction();
        mascot->queued_behavior = mascot->interaction.behavior();
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BROADCASTS, "Client did meet server, starting interaction");
            log(SHIJIMA_LOG_BROADCASTS, "Queued behavior: " + mascot->queued_behavior);
        #endif
        return true;
    }
    return ret;
}

}
}