#include "interact.hpp"

namespace shijima {
namespace action {

bool interact::tick() {
    if (!animate::tick()) {
        if (animation_finished()) {
            auto next = vars.get_string("Behavior");
            if (next != "") {
                mascot->queued_behavior = next;
                return true;
            }
        }
        return false;
    }
    return mascot->interaction.ongoing();
}

}
}