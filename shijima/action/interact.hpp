#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class interact : public animate {
public:
    virtual bool tick() override {
        if (!animate::tick()) {
            if (animation_finished()) {
                //TODO: Continue with the behavior specified in the Interact action (attr "Behavior")
            }
            return false;
        }
        return mascot->interaction.ongoing();
    }
};

}
}