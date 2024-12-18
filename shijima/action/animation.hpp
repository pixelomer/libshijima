#pragma once
#include "base.hpp"
#include <shijima/animation.hpp>

namespace shijima {
namespace action {

class animation : public base {
protected:
    int anim_idx;
    virtual std::shared_ptr<shijima::animation> &get_animation();
    pose const& get_pose();
    bool animation_finished();
public:
    virtual bool requests_broadcast() override;
    std::vector<std::shared_ptr<shijima::animation>> animations;
    virtual void init(mascot::tick &ctx) override;
    virtual bool tick() override;
};

}
}