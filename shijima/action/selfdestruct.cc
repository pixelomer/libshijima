#include "selfdestruct.hpp"

namespace shijima {
namespace action {

bool selfdestruct::tick() {
    if (!animate::tick()) {
        if (animation_finished()) {
            mascot->dead = true;
            return true;
        }
        return false;
    }
    return true;
}

}
}