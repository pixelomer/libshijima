#pragma once
#include "instant.hpp"

namespace shijima {
namespace action {

class offset : public instant {
private:
    double target_x, target_y;
public:
    virtual bool tick() override {
        if (first_subtick()) {
            target_x = vars.get_num("X", 0);
            target_y = vars.get_num("Y", 0);
        }
        double dt = 1.0 / mascot->env->subticks_per_tick;
        mascot->anchor.x += target_x * dt;
        mascot->anchor.y += target_y * dt;
        return instant::tick();
    }
};

}
}