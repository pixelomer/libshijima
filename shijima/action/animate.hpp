#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class animate : public animation {
public:
    virtual bool tick() override {
        if (!animation::tick()) {
            return false;
        }
        return get_animation()->get_duration() > elapsed();
    }
};

}
}