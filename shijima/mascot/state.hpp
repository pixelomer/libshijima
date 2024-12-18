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

    void roll_dcursor();
    environment::dvec2 &get_raw_cursor();
    environment::dvec2 get_cursor();
    bool on_land();
};

}
}