#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class resist : public animate {
public:
    virtual bool tick() override {
        if (std::abs(mascot->get_cursor().x - mascot->anchor.x) >= 5) {
            // Cursor moved, abort without escaping
            return false;
        }
        bool ret = animate::tick();
        if (animation_finished()) {
            // Animation concluded, escape
            mascot->dragging = false;
            return false;
        }
        return ret;
    }
};

}
}