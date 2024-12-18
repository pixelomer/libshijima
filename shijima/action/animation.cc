#include "animation.hpp"

namespace shijima {
namespace action {

bool animation::requests_broadcast() {
    return true;
}

void animation::init(mascot::tick &ctx) {
    base::init(ctx);
    anim_idx = -1;
}

bool animation::tick() {
    if (!base::tick()) {
        return false;
    }
    auto &pose = get_pose();
    mascot->anchor.x += dx(pose.velocity.x);
    mascot->anchor.y += dy(pose.velocity.y);
    mascot->active_frame = pose;
    return true;
}

std::shared_ptr<shijima::animation> &animation::get_animation() {
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

pose const& animation::get_pose() {
    return get_animation()->get_pose(elapsed());
}

bool animation::animation_finished() {
    return elapsed() >= get_animation()->get_duration();
}

}
}