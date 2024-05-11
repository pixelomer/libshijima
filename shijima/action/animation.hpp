#pragma once
#include "base.hpp"

namespace shijima {
namespace action {

//FIXME: action::animation should inherit action::bordered

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
public:
    std::vector<std::shared_ptr<shijima::animation>> animations;
    virtual void init(scripting::context &script_ctx,
        std::map<std::string, std::string> const& extra)
    {
        base::init(script_ctx, extra);
        anim_idx = -1;
    }
    virtual bool tick() {
        if (!base::tick()) {
            return false;
        }
        auto &pose = get_pose();
        mascot->active_frame = pose;
        mascot->anchor.x += dx(pose.velocity.x);
        mascot->anchor.y += dy(pose.velocity.y);
        return true;
    }
};

}
}