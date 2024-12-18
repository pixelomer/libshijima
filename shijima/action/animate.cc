#include "animate.hpp"

namespace shijima {
namespace action {

bool animate::tick() {
    if (!animation::tick()) {
        return false;
    }
    return !animation_finished();
}

}
}