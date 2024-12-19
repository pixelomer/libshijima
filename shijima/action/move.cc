#include "move.hpp"
#include <stdexcept>

namespace shijima {
namespace action {

// "目的地Y" and "目的地X" are required for backwards compatibility

bool move::tick() {
    if (vars.has("TargetX")) {
        double x = vars.get_num("TargetX");
        vars.add_attr({{ "目的地X", x }});
        auto &pose = get_pose();
        if (pose.velocity.x > 0) {
            mascot->looking_right = (x < mascot->anchor.x);
        }
        else if (pose.velocity.x < 0) {
            mascot->looking_right = (x > mascot->anchor.x);
        }
    }
    if (vars.has("TargetY")) {
        vars.add_attr({{ "目的地Y", vars.get_num("TargetY") }});
        if (mascot->env->work_area.left_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.right_border().is_on(mascot->anchor))
        {
            mascot->looking_right = false;
        }
        if (mascot->env->work_area.right_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.left_border().is_on(mascot->anchor))
        {
            mascot->looking_right = true;
        }
    }

    auto start = mascot->anchor;
    if (!animation::tick()) {
        return false;
    }
    auto end = mascot->anchor;
    #define passed(x) (start.x >= x && end.x <= x) || \
        (start.x <= x && end.x >= x)
    if (vars.has("TargetX")) {
        double x = vars.get_num("TargetX");
        if (passed(x)) {
            mascot->anchor.x = x;
            return false;
        }
    }
    else if (vars.has("TargetY")) {
        double y = vars.get_num("TargetY");
        if (passed(y)) {
            mascot->anchor.y = y;
            return false;
        }
    }
    else {
        throw std::logic_error("Neither TargetX nor TargetY defined");
    }
    return true;
}

}
}