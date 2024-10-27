#pragma once
#include <memory>
#include <array>
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
    long time = 0;
    long subtick = 0;
    long drag_lock;
    bool can_breed = true;
    int dcursor_idx = 0;
    std::vector<math::vec2> dcursor_buffer;
    
    void reset_dcursor_buffer() {
        dcursor_buffer.clear();
        for (int i=0; i<env->subticks_per_tick * 5; ++i) {
            dcursor_buffer.push_back(math::vec2::nan());
        }
        dcursor_idx = 0;
    }
    
    math::vec2 get_dcursor() {
        int count = 0;
        math::vec2 vec;
        for (int i=dcursor_idx+1, j=1; i != dcursor_idx;
            i = (i+1) % dcursor_buffer.size(), ++j)
        {
            auto const& dcursor = dcursor_buffer[i];
            double mult = dcursor_buffer.size() - j;
            mult = 1 / (mult * mult);
            if (!dcursor.isnan()) {
                ++count;
                vec += dcursor * mult;
            }
        }
        if (count == 0) {
            return { 0, 0 };
        }
        return vec * env->throw_power;
    }

    void push_dcursor(math::vec2 dcursor) {
        dcursor_buffer[dcursor_idx] = dcursor;
        dcursor_idx = (dcursor_idx + 1) % dcursor_buffer.size();
    }

    bool new_tick() {
        return subtick == 0;
    }

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