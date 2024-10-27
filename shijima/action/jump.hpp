#pragma once
#include "move.hpp"
#include <cmath>

namespace shijima {
namespace action {

class jump : public animation {
private:
    math::vec2 velocity;
public:
    virtual bool tick() override {
        if (mascot->new_tick()) {
            math::vec2 target { vars.get_num("TargetX", 0),
                vars.get_num("TargetY", 0) };
            mascot->looking_right = mascot->anchor.x < target.x;
            math::vec2 distance;
            distance.x = target.x - mascot->anchor.x,
            distance.y = target.y - mascot->anchor.y - std::abs(distance.x);
            double velocity_abs = vars.get_num("VelocityParam", 20);
            double distance_abs = sqrtf(distance.x*distance.x + distance.y*distance.y);
            if (distance_abs != 0) {
                velocity = { velocity_abs * distance.x / distance_abs,
                    velocity_abs * distance.y / distance_abs };
            }
            if (distance_abs <= velocity_abs) {
                mascot->anchor = target;
                return false;
            }
        }
        mascot->anchor.x += velocity.x * dt();
        mascot->anchor.y += velocity.y * dt();
        return animation::tick();
    }
};

}
}