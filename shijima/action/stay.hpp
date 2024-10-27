#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class stay : public animation {
private:
    int duration;
public:
    virtual bool tick() override {
        if (mascot->new_tick()) {
            duration = (int)vars.get_num("Duration", 250);
        }
        if (!animation::tick()) {
            return false;
        }
        return elapsed() < duration;
    }
};

}
}