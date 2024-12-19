#include "dragged.hpp"

namespace shijima {
namespace action {

void dragged::init(mascot::tick &ctx) {
    animation::init(ctx);
    auto offset_x = vars.get_num("OffsetX", 0);
    foot_dx = 0;
    foot_x = mascot->get_cursor().x + offset_x;
    time_to_resist = 250;
    m_prevents_dragging = true;
    vars.add_attr({ { "FootX", foot_x }, { "footX", foot_x } });
}

bool dragged::requests_interpolation() {
    return false;
}

bool dragged::subtick(int idx) {
    mascot->looking_right = false;
    auto cursor = mascot->get_cursor();
    if (idx == 0) {
        if (!mascot->dragging) {
            // Stopped dragging
            mascot->queued_behavior = "Thrown";
            mascot->was_on_ie = false;
            return false;
        }
        foot_dx = (foot_dx + ((cursor.x - foot_x) * 0.1)) * 0.8;
        foot_x += foot_dx;
        vars.add_attr({ { "FootX", foot_x }, { "footX", foot_x },
            { "FootDX", foot_dx }, { "footDX", foot_dx } });
    }
    auto offset_x = vars.get_num("OffsetX", 0);
    auto offset_y = vars.get_num("OffsetY", 120);
    auto subtick_count = mascot->env->subtick_count;
    if (std::abs(cursor.x - mascot->anchor.x + offset_x) >= (5.0 / subtick_count)) {
        reset_elapsed();
    }
    if (!animation::subtick(idx)) {
        return false;
    }
    mascot->anchor.x = cursor.x + offset_x;
    mascot->anchor.y = cursor.y + offset_y;
    if (elapsed() >= time_to_resist) {
        return false;
    }
    return true;
}

}
}