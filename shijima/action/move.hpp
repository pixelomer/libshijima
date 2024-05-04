#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class move : public animation {
public:
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra)
    {
        animation::init(mascot, extra);
    }
    virtual bool tick() {
        if (vars.has("TargetX")) {
            int x = vars.get_num("TargetX");
            auto &pose = get_pose();
            if (pose.velocity.x > 0) {
                mascot->looking_right = (x < mascot->anchor.x);
            }
            else if (pose.velocity.x < 0) {
                mascot->looking_right = (x > mascot->anchor.x);
            }
        }
        if (vars.has("TargetY")) {
            if (mascot->env.work_area.left_border().is_on(mascot->anchor) ||
                mascot->env.active_ie.right_border().is_on(mascot->anchor))
            {
                mascot->looking_right = false;
            }
            if (mascot->env.work_area.right_border().is_on(mascot->anchor) ||
                mascot->env.active_ie.left_border().is_on(mascot->anchor))
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
            int x = vars.get_num("TargetX");
            if (passed(x)) {
                mascot->anchor.x = x;
                return false;
            }
        }
        else if (vars.has("TargetY")) {
            int y = vars.get_num("TargetY");
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
};

}
}