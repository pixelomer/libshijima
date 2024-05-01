#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class fall : public animation {
    virtual bool tick() {
        if (!animation::tick()) {
            return false;
        }
        //FIXME: Most arguments ignored
        int gravity = (int)vars.get_num("Gravity", 2);
        mascot->anchor.y += gravity;
        if (mascot->anchor.y >= mascot->env.work_area.bottom) {
            mascot->anchor.y = mascot->env.work_area.bottom;
            return false;
        }
        return true;
    }
};

}
}