#pragma once
#include "move.hpp"

namespace shijima {
namespace action {

class movewithturn : public move {
protected:
    bool is_turning = false;
    bool headed_right() {
        double target_x = vars.get_num("TargetX");
        return mascot->anchor.x <= target_x;
    }
    bool needs_turn() {
        return (mascot->looking_right && !headed_right()) ||
            (!mascot->looking_right && headed_right());
    }
    virtual std::shared_ptr<shijima::animation> &get_animation() override {
        if (animations.size() < 2) {
            return move::get_animation();
        }
        if (!is_turning && needs_turn()) {
            is_turning = true;
            mascot->looking_right = headed_right();
            start_time = mascot->time;
        }
        else if (is_turning && elapsed() >= animations[1]->get_duration()) {
            is_turning = false;
            start_time = mascot->time;
        }
        return animations[is_turning ? 1 : 0];
    }
public:
    virtual void init(mascot::tick &ctx) override {
        move::init(ctx);
        #ifdef SHIJIMA_LOGGING_ENABLED
            if (animations.size() != 2) {
                log(SHIJIMA_LOG_WARNINGS,
                    "warning: expected 2 animations for MoveWithTurn, got "
                    + std::to_string(animations.size()) + " animations");
            }
        #endif
    }
};

}
}