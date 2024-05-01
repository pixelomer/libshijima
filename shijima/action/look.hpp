#pragma once
#include "instant.hpp"

namespace shijima {
namespace action {

class look : public instant {
public:
    virtual bool tick() {
        mascot->looking_right = vars.get_bool("LookRight", false);
        return false;
    }
};

}
}