#pragma once
#include "move.hpp"

namespace shijima {
namespace action {

class movewithturn : public move {
protected:
    bool is_turning = false;
    bool headed_right();
    bool needs_turn();
    virtual std::shared_ptr<shijima::animation> &get_animation() override;
public:
    virtual void init(mascot::tick &ctx) override;
};

}
}