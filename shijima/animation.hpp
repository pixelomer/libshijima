#pragma once
#include <vector>
#include "scripting/condition.hpp"
#include "pose.hpp"
#include "hotspot.hpp"

namespace shijima {

class animation {
private:
    std::vector<pose> poses;
    std::vector<hotspot> hotspots;
    int duration = 0;
public:
    scripting::condition condition;
    // time is 0-indexed. The first frame happens at t=0
    pose const& get_pose(int time);
    std::string hotspot_behavior_at(math::vec2 offset);
    int get_duration();
    animation(std::vector<shijima::pose> const& poses,
        std::vector<shijima::hotspot> const& hotspots);
};

}