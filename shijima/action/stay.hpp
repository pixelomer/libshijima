#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class stay : public animation {
public:
    virtual bool tick() override {
        if (!animation::tick()) {
            return false;
        }
        return elapsed() <= vars.get_num("Duration", 250);
    }
};

}
}