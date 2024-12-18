#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class fall : public animation {
protected:
    math::vec2 velocity;
public:
    virtual void init(mascot::tick &ctx) override;
    virtual bool tick() override;
};

}
}