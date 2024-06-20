#pragma once
#include <memory>
#include <shijima/math.hpp>
#include <shijima/pose.hpp>
#include "environment.hpp"

namespace shijima {
namespace mascot {

class state {
public:
    struct breed_request_data {
        bool available = false;
        bool transient;
        bool looking_right;
        math::vec2 anchor;
        std::string name; // empty if self
        std::string behavior;
    };

    breed_request_data breed_request;

    math::rec bounds;
    math::vec2 anchor;
    shijima::frame active_frame;
    std::shared_ptr<environment> env;
    std::map<std::string, std::string> constants;
    bool looking_right;
    bool dragging;
    long time;

    bool drag_with_local_cursor;
    environment::dvec2 local_cursor;

    environment::dvec2 &get_cursor() {
        if (dragging && drag_with_local_cursor) {
            return local_cursor;
        }
        else {
            return env->cursor;
        }
    }
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