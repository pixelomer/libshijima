#pragma once
#include <shijima/math.hpp>
#include <shijima/pose.hpp>
#include "environment.hpp"

namespace shijima {
namespace mascot {

class state {
public:
    math::rec bounds;
    math::vec2 anchor;
    shijima::frame active_frame;
    std::shared_ptr<environment> env;
    bool looking_right;
    bool dragging;
    long time;

    bool is_on(environment::area &area) {
        return area.left_border().is_on(anchor) ||
            area.right_border().is_on(anchor) ||
            area.bottom_border().is_on(anchor) ||
            area.top_border().is_on(anchor);
    }
    bool on_land() {
        return env->floor.is_on(anchor) ||
            env->ceiling.is_on(anchor) ||
            is_on(env->work_area) || is_on(env->active_ie);
    }
};

}
}