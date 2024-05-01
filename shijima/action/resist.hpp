#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class resist : public animate {
public:
    virtual bool tick() {
        if (!animate::tick()) {
            // Animation concluded, escape
            mascot->dragging = false;
            return false;
        }
        return true;
    }
};

}
}