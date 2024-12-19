#include "animate.hpp"

namespace shijima {
namespace action {

bool animate::tick() {
    if (animation_finished()) {
        return false;
    }
    return animation::tick();
}

}
}