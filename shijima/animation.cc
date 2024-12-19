#include "animation.hpp"
#include <stdexcept>

namespace shijima {

pose const& animation::get_pose(int time) {
    time %= duration;
    for (auto const& pose : poses) {
        time -= pose.duration;
        if (time < 0) {
            return pose;
        }
    }
    throw std::logic_error("impossible condition");
}

int animation::get_duration() {
    return duration;
}

animation::animation(std::vector<shijima::pose> const& poses): poses(poses),
    condition(true)
{
    for (auto const& pose : poses) {
        duration += pose.duration;
    }
}

}