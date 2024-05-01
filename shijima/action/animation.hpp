#pragma once
#include "base.hpp"

namespace shijima {
namespace action {

//FIXME: action::animation should inherit action::bordered

class animation : public base {
protected:
    int anim_idx;

    std::shared_ptr<shijima::animation> &get_animation() {
        for (int i=0; i<animations.size(); i++) {
            auto &anim = animations[anim_idx];
            bool valid;
            if (anim->condition == "") {
                valid = true;
            }
            else {
                vars.add_attr({{ "_Condition", ((anim->condition == "") ?
                    "${true}" : anim->condition) }});
                valid = vars.get_bool("_Condition");
            }
            if (valid) {
                if (anim_idx != i) {
                    anim_idx = i;
                    start_time = mascot->time;
                }
                return anim;
            }
        }
        throw std::logic_error("no animation available");
    }
public:
    std::vector<std::shared_ptr<shijima::animation>> animations;
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra)
    {
        base::init(mascot, extra);
        anim_idx = -1;
    }
    virtual bool tick() {
        if (!base::tick()) {
            return false;
        }
        auto &anim = get_animation();
        auto &pose = anim->get_pose(elapsed());
        mascot->active_frame = pose;
        mascot->anchor.x += pose.velocity.x;
        mascot->anchor.y += pose.velocity.y;
        return true;
    }
};

}
}