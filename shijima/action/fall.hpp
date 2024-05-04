#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class fall : public animation {
    virtual bool tick() {
        if (!animation::tick()) {
            return false;
        }
        if (mascot->env.floor.is_on(mascot->anchor) ||
            mascot->env.floor.is_on(mascot->anchor))
        {
            return false;
        }
        int gravity = (int)vars.get_num("Gravity", 5);
        mascot->anchor.y += gravity;
        if (mascot->anchor.y >= mascot->env.floor.y) {
            mascot->anchor.y = mascot->env.floor.y;
            return false;
        }
        return true;
    }
};

}
}