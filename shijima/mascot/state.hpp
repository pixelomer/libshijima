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
    std::string active_sound;
    bool active_sound_changed;
    bool looking_right;
    bool dragging;
    bool was_on_ie;
    bool dead;
    long time;
    long drag_lock;
    bool can_breed = true;

    bool drag_with_local_cursor;
    environment::dvec2 local_cursor;

    std::vector<math::vec2> stored_dcursor_data;
    environment::dvec2 stored_dcursor;
    int next_dcursor_roll;

    void roll_dcursor() {
        if ((int)stored_dcursor_data.size() != env->subtick_count) {
            stored_dcursor = { 0, 0, 0, 0 };
            stored_dcursor_data.resize(env->subtick_count);
            for (auto &vec : stored_dcursor_data) {
                vec = { 0, 0 };
            }
            next_dcursor_roll = 0;
        }
        auto raw_cursor = get_raw_cursor();
        stored_dcursor_data[next_dcursor_roll] = { raw_cursor.dx,
            raw_cursor.dy };
        
        stored_dcursor = { raw_cursor.x, raw_cursor.y, 0, 0 };
        for (auto const& vec : stored_dcursor_data) {
            stored_dcursor.dx += vec.x;
            stored_dcursor.dy += vec.y;
        }
        next_dcursor_roll = (next_dcursor_roll + 1) % env->subtick_count;
    }

    environment::dvec2 &get_raw_cursor() {
        if (dragging && drag_with_local_cursor) {
            return local_cursor;
        }
        else {
            return env->cursor;
        }
    }

    environment::dvec2 get_cursor() {
        environment::dvec2 cursor;
        if (dragging && drag_with_local_cursor) {
            cursor = local_cursor;
        }
        else {
            cursor = env->cursor;
        }
        return stored_dcursor;
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