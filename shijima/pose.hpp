#pragma once

// 
// libshijima - C++ library for shimeji desktop mascots
// Copyright (C) 2024-2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#include "config.hpp"
#include <string>
#include "math.hpp"
#include <cereal/types/string.hpp>

namespace shijima {

class frame {
public:
    bool visible;
    std::string name;
    std::string right_name;
    std::string sound;
    math::vec2 anchor;
    frame(std::string const& name, std::string const& right_name,
        std::string const& sound, math::vec2 anchor): visible(true), name(name),
        right_name(right_name), sound(sound), anchor(anchor) {}
    frame(): visible(false) {}
    std::string const& get_name(bool right) {
        return (right && !right_name.empty()) ? right_name : name;
    }
    bool operator<(const frame& rhs) const {
        return name < rhs.name;
    }
    bool operator>(const frame& rhs) const {
        return name > rhs.name;
    }
    bool operator==(const frame& rhs) const {
        return name == rhs.name;
    }
    bool operator!=(const frame& rhs) const {
        return !(name == rhs.name);
    }
};

class pose : public frame {
public:
    math::vec2 velocity;
    int duration;
    pose(std::string const& name, std::string const& right_name, std::string const& sound,
        math::vec2 anchor, math::vec2 velocity, int duration):
        frame(name, right_name, sound, anchor), velocity(velocity), duration(duration) {}
    pose(): frame() {}

    template<class Archive>
    void serialize(Archive &ar) {
        ar(visible, name, right_name, sound, anchor, velocity, duration);
    }
};

}
