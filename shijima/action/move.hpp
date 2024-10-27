#pragma once
#include "animation.hpp"

// "目的地Y" and "目的地X" are required for backwards compatibility

namespace shijima {
namespace action {

class move : public animation {
private:
    double target_x;
    double target_y;
public:
    virtual void init(mascot::tick &ctx) override {
        animation::init(ctx);
        target_x = target_y = NAN;
    }
    virtual bool tick() override {
        if (mascot->new_tick()) {
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
                target_x = vars.get_num("TargetX");
            }
            else {
                target_x = NAN;
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
                target_y = vars.get_num("TargetY");
            }
            else {
                target_y = NAN;
            }
        }
        auto start = mascot->anchor;
        if (!animation::tick()) {
            return false;
        }
        auto end = mascot->anchor;
        #define passed(x) (start.x >= x && end.x <= x) || \
            (start.x <= x && end.x >= x)
        if (!std::isnan(target_x)) {
            double x = target_x;
            if (passed(x)) {
                mascot->anchor.x = x;
                return !mascot->new_tick();
            }
        }
        else if (!std::isnan(target_y)) {
            double y = target_y;
            if (passed(y)) {
                mascot->anchor.y = y;
                return !mascot->new_tick();
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