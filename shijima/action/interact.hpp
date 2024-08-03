#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class interact : public animate {
public:
    virtual bool tick() override {
        if (!animate::tick()) {
            if (animation_finished()) {
                auto next = vars.get_string("Behavior");
                if (next != "") {
                    mascot->queued_behavior = next;
                    return true;
                }
            }
            return false;
        }
        return mascot->interaction.ongoing();
    }
};

}
}