#pragma once
#include "animation.hpp"

// "footX" is required for backwards compatibility

namespace shijima {
namespace action {

class dragged : public animation {
protected:
    double foot_x;
    double foot_dx;
    int time_to_resist;
    virtual bool handle_dragging() override;
public:
    virtual bool requests_interpolation() override;
    virtual void init(mascot::tick &ctx) override;
    virtual bool subtick(int idx) override;
};

}
}