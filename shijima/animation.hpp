#pragma once
#include <string>
#include <vector>
#include "pose.hpp"
#include "math.hpp"

namespace shijima {

class animation {
private:
    std::vector<pose> poses;
    int duration = 0;
public:
    std::string condition;
    // time is 0-indexed. The first frame happens at t=0
    pose const& get_pose(int time) {
        time %= duration;
        for (auto const& pose : poses) {
            time -= pose.duration;
            if (time < 0) {
                return pose;
            }
        }
        throw std::logic_error("impossible condition");
    }
    int get_duration() {
        return duration;
    }
    animation(std::vector<shijima::pose> const& poses): poses(poses) {
        for (auto const& pose : poses) {
            duration += pose.duration;
        }
    }
};

}