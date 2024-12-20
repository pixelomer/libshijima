#include "resist.hpp"

namespace shijima {
namespace action {

bool resist::handle_dragging() {
    if (!mascot->dragging) {
        // Stopped dragging
        mascot->queued_behavior = "Thrown";
        mascot->was_on_ie = false;
        return false;
    }
    return true;
}

bool resist::tick() {
    if (std::abs(mascot->get_cursor().x - mascot->anchor.x) >= 5) {
        // Cursor moved, abort without escaping
        mascot->queued_behavior = "Dragged";
        mascot->was_on_ie = false;
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