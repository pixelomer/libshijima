#pragma once
#include "instant.hpp"

namespace shijima {
namespace action {

class offset : public instant {
public:
    virtual bool tick() override {
        mascot->anchor.x += (int)vars.get_num("X", 0);
        mascot->anchor.y += (int)vars.get_num("Y", 0);
        return false;
    }
};

}
}