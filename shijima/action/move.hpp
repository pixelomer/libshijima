#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class move : public animation {
public:
    virtual bool tick() {
        if (!animation::tick()) {
            return false;
        }
        if (vars.has("TargetX") && (int)vars.get_num("TargetX") != mascot->anchor.x) {
            return true;
        }
        if (vars.has("TargetY") && (int)vars.get_num("TargetY") != mascot->anchor.y) {
            return true;
        }
        return false;
    }
};

}
}