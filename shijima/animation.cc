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

animation::animation(std::vector<shijima::pose> const& poses,
    std::vector<shijima::hotspot> const& hotspots): poses(poses),
    hotspots(hotspots), condition(true)
{
    for (auto const& pose : poses) {
        duration += pose.duration;
    }
}

hotspot animation::hotspot_at(math::vec2 offset) {
    for (auto const& hotspot : hotspots) {
        if (hotspot.point_inside(offset)) {
            return hotspot;
        }
    }
    return {};
}

}