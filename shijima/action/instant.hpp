#pragma once
#include "base.hpp"

namespace shijima {
namespace action {

class instant : public base {
public:
    virtual bool tick() override {
        return elapsed() == 0;
    }
};

}
}