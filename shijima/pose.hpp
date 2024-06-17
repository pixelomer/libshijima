#pragma once
#include <string>
#include "math.hpp"

namespace shijima {

class frame {
public:
    bool visible;
    std::string name;
    math::vec2 anchor;
    frame(std::string const& name, math::vec2 anchor): visible(true),
        name(name), anchor(anchor) {}
    frame(): visible(false) {}
    bool operator<(const frame& rhs) const {
        return name < rhs.name;
    }
    bool operator>(const frame& rhs) const {
        return name > rhs.name;
    }
};

class pose : public frame {
public:
    math::vec2 velocity;
    int duration;
    pose(std::string const& name, math::vec2 anchor, math::vec2 velocity,
        int duration): frame(name, anchor), velocity(velocity),
        duration(duration) {}
    pose(): frame() {}
};

}