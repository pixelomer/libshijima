#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class fall : public animation {
protected:
    math::vec2 velocity;
public:
    virtual bool requests_interpolation() override;
    virtual void init(mascot::tick &ctx) override;
    virtual bool subtick(int idx) override;
};

}
}