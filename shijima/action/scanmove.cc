#include "scanmove.hpp"
#include <cmath>
#include <shijima/log.hpp>

namespace shijima {
namespace action {

bool scanmove::requests_broadcast() {
    return false;
}

void scanmove::init(mascot::tick &ctx) {
    move::init(ctx);
    mascot->env->broadcasts.try_connect(client, mascot->anchor.y,
        vars.get_string("Affordance"), vars.get_string("Behavior"),
        vars.get_string("TargetBehavior"));
}

bool scanmove::tick() {
    if (!client.connected()) {
        return false;
    }
    vars.add_attr({ { "TargetX", client.get_target().x } });
    bool ret = move::tick();
    auto target = client.get_target();
    if (std::fabs(mascot->anchor.x - target.x) < 3) {
        mascot->anchor.x = target.x;
        client.notify_arrival();
        mascot->interaction = client.get_interaction();
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