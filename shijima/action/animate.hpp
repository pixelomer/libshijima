#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class animate : public animation {
public:
    virtual bool tick() override;
};

}
}