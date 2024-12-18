#include "resist.hpp"

namespace shijima {
namespace action {

bool resist::tick() {
    if (std::abs(mascot->get_cursor().x - mascot->anchor.x) >= 5) {
        // Cursor moved, abort without escaping
        return false;
    }
    bool ret = animate::tick();
    if (animation_finished()) {
        // Animation concluded, escape
        mascot->dragging = false;
        return false;
    }
    return ret;
}

}
}