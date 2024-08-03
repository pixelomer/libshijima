#pragma once
#include <memory>
#include <shijima/math.hpp>
#include <shijima/pose.hpp>
#include <shijima/broadcast/interaction.hpp>
#include "environment.hpp"

namespace shijima {
namespace mascot {

class state {
public:
    class breed_request_data {
    public:
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
    broadcast::interaction interaction;
    std::string queued_behavior;
    bool looking_right;
    bool dragging;
    bool was_on_ie;
    bool dead;
    long time;
    long drag_lock;

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
    bool on_land() {
        return env->floor.is_on(anchor) ||
            env->ceiling.is_on(anchor) ||
            env->work_area.is_on(anchor) ||
            env->active_ie.is_on(anchor);
    }
};

}
}