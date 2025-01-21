#include "fall.hpp"

namespace shijima {
namespace action {

void fall::init(mascot::tick &ctx) {
    animation::init(ctx);
    velocity.x = vars.get_num("InitialVX", 0);
    velocity.y = vars.get_num("InitialVY", 0);
}

bool fall::requests_interpolation() {
    return false;
}

bool fall::subtick(int idx) {
    if (!animation::subtick(idx)) {
        return false;
    }
    bool on_land = mascot->env->floor.is_on(mascot->anchor) ||
        mascot->env->ceiling.is_on(mascot->anchor) ||
        mascot->env->work_area.is_on(mascot->anchor);
    if (elapsed() > 0) {
        // Don't consider IE on the first tick
        on_land = on_land ||
            mascot->env->active_ie.is_on(mascot->anchor);
    }
    if (on_land) {
        return false;
    }

    if (velocity.x != 0) {
        mascot->looking_right = (velocity.x > 0);
    }

    auto subtick_count = mascot->env->subtick_count;

    math::vec2 resistance;
    resistance.x = vars.get_num("RegistanceX", 0.05);
    resistance.y = vars.get_num("RegistanceY", 0.1);
    double gravity = vars.get_num("Gravity", 2);
    velocity.x -= (velocity.x * resistance.x) / subtick_count;
    velocity.y += (gravity - velocity.y * resistance.y) / subtick_count;
    
    vars.add_attr({{ "VelocityX", velocity.x }, { "VelocityY", velocity.y }});

    math::vec2 before = mascot->anchor;

    mascot->anchor.x += velocity.x / subtick_count;
    mascot->anchor.y += velocity.y / subtick_count;
    
    if (mascot->anchor.x > mascot->env->work_area.right) {
        mascot->anchor.x = mascot->env->work_area.right;
    }
    else if (mascot->anchor.x < mascot->env->work_area.left) {
        mascot->anchor.x = mascot->env->work_area.left;
    }
    if (mascot->anchor.y < mascot->env->ceiling.y) {
        mascot->anchor.y = mascot->env->ceiling.y;
    }
    else if (mascot->anchor.y > mascot->env->floor.y) {
        mascot->anchor.y = mascot->env->floor.y;
    }

    math::vec2 after = mascot->anchor;

    #define AREA_STICK(area, axis, side, before_comp, after_comp) \
            (mascot->env->area.visible() && \
            mascot->env->area.side##_border().faces(after) && \
            before.axis before_comp mascot->env->area.side && \
            after.axis after_comp mascot->env->area.side) \
        do { mascot->anchor.axis = mascot->env->area.side; } \
        while (0)

    #define IE_STICK(axis, side, before_comp, after_comp) \
        AREA_STICK(active_ie, axis, side, before_comp, after_comp)
    
    if      IE_STICK(x, left, <=, >=);
    else if IE_STICK(x, right, >=, <=);
    else if IE_STICK(y, top, <=, >=);
    else if IE_STICK(y, bottom, >=, <=);

    #undef IE_STICK
    #undef AREA_STICK
    
    return true;
}

}
}