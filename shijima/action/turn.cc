#include "turn.hpp"

namespace shijima {
namespace action {

bool turn::tick() {
    bool will_look_right = vars.get_bool("LookRight", false);
    if (elapsed() == 0) {
        // First tick
        if ((will_look_right && mascot->looking_right) ||
            (!will_look_right && !mascot->looking_right))
        {
            // Turn is unnecessary
            return false;
        }
        else {
            mascot->looking_right = will_look_right;
        }
    }
    return animate::tick();
}

}
}