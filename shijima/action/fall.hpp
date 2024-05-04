#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class fall : public animation {
protected:
    double modf(double a, double b) {
        return std::modf(a, &b);
    }
    math::vec2 velocity;
public:
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra)
    {
        animation::init(mascot, extra);
        velocity.x = vars.get_num("InitialVX", 0);
        velocity.y = vars.get_num("InitialVY", 0);
    }

    virtual bool tick() {
        if (mascot->on_land()) {
            return false;
        }
        if (velocity.x != 0) {
            mascot->looking_right = (velocity.x > 0);
        }

        math::vec2 resistance;
        resistance.x = vars.get_num("RegistanceX", 0.05);
        resistance.y = vars.get_num("RegistanceY", 0.1);
        double gravity = vars.get_num("Gravity", 2);
        velocity.x -= velocity.x * resistance.x;
        velocity.y += gravity - velocity.y * resistance.y;
        
        vars.add_attr({{ "VelocityX", velocity.x }, { "VelocityY", velocity.y }});

        mascot->anchor.x += (int)velocity.x;
        mascot->anchor.y += (int)velocity.y;

        if (mascot->anchor.x > mascot->env.work_area.right) {
            mascot->anchor.x = mascot->env.work_area.right;
        }
        else if (mascot->anchor.x < mascot->env.work_area.left) {
            mascot->anchor.x = mascot->env.work_area.left;
        }
        if (mascot->anchor.y < mascot->env.ceiling.y) {
            mascot->anchor.y = mascot->env.ceiling.y;
        }
        else if (mascot->anchor.y > mascot->env.floor.y) {
            mascot->anchor.y = mascot->env.floor.y;
        }
        
        if (!animation::tick()) {
            return false;
        }
        return true;
    }
};

}
}