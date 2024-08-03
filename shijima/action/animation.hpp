#pragma once
#include "base.hpp"

namespace shijima {
namespace action {

class animation : public base {
protected:
    int anim_idx;

    std::shared_ptr<shijima::animation> &get_animation() {
        for (int i=0; i<(int)animations.size(); i++) {
            auto &anim = animations[i];
            if (vars.get_bool(anim->condition)) {
                if (anim_idx != i) {
                    anim_idx = i;
                    start_time = mascot->time;
                }
                return anim;
            }
        }
        throw std::logic_error("no animation available");
    }

    pose const& get_pose() {
        return get_animation()->get_pose(elapsed());
    }
    
    bool animation_finished() {
        return elapsed() >= get_animation()->get_duration();
    }
public:
    virtual bool requests_broadcast() override {
        return true;
    }
    std::vector<std::shared_ptr<shijima::animation>> animations;
    virtual void init(mascot::tick &ctx) override {
        base::init(ctx);
        anim_idx = -1;
    }
    virtual bool tick() override {
        if (!base::tick()) {
            return false;
        }
        auto &pose = get_pose();
        mascot->anchor.x += dx(pose.velocity.x);
        mascot->anchor.y += dy(pose.velocity.y);
        mascot->active_frame = pose;
        return true;
    }
};

}
}