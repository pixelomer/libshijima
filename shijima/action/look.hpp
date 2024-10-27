#pragma once
#include "instant.hpp"

namespace shijima {
namespace action {

class look : public instant {
private:
    bool will_look_right;
public:
    virtual bool tick() override {
        if (first_subtick()) {
            will_look_right = vars.get_bool("LookRight", false);
        }
        mascot->looking_right = will_look_right;
        return instant::tick();
    }
};

}
}