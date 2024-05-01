#pragma once
#include "move.hpp"

namespace shijima {
namespace action {

class jump : public move {
private:
    static double absceil(double val) {
        if (val >= 0) return std::ceil(val);
        else return -std::ceil(std::abs(val));
    }
public:
    virtual bool tick() {
        if (!move::tick()) {
            return false;
        }
        double tx = vars.get_num("TargetX", 0);
        double ty = vars.get_num("TargetY", 0);
        double dx = mascot->anchor.x - tx;
        double dy = mascot->anchor.y - ty;
        double distance = std::sqrt(dx*dx + dy*dy); // will never be 0
        double vel = vars.get_num("VelocityParam", 20);
        mascot->anchor.x += (int)absceil(vel * dx/distance);
        mascot->anchor.y += (int)absceil(vel * dx/distance);
        return true;
    }
};

}
}