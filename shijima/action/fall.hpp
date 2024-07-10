#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class fall : public animation {
protected:
    math::vec2 velocity;
public:
    virtual void init(mascot::tick &ctx) override {
        animation::init(ctx);
        velocity.x = vars.get_num("InitialVX", 0);
        velocity.y = vars.get_num("InitialVY", 0);
    }

    virtual bool tick() override {
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
        
        if (!animation::tick()) {
            return false;
        }
        return true;
    }
};

}
}