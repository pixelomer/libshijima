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
    environment env;
    bool looking_right;
    bool dragging;
    long time;
};

}
}