#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class stay : public animation {
public:
    virtual bool tick() {
        if (!animation::tick()) {
            return false;
        }
        return elapsed() <= vars.get_num("Duration", 250);
    }
};

}
}