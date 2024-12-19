#pragma once
#include <vector>
#include "scripting/condition.hpp"
#include "pose.hpp"

namespace shijima {

class animation {
private:
    std::vector<pose> poses;
    int duration = 0;
public:
    scripting::condition condition;
    // time is 0-indexed. The first frame happens at t=0
    pose const& get_pose(int time);
    int get_duration();
    animation(std::vector<shijima::pose> const& poses);
};

}