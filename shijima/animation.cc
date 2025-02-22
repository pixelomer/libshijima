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

#include "animation.hpp"
#include <stdexcept>

namespace shijima {

pose const& animation::get_pose(int time) {
    time %= duration;
    for (auto const& pose : poses) {
        time -= pose.duration;
        if (time < 0) {
            return pose;
        }
    }
    throw std::logic_error("impossible condition");
}

int animation::get_duration() {
    return duration;
}

animation::animation(std::vector<shijima::pose> const& poses,
    std::vector<shijima::hotspot> const& hotspots): poses(poses),
    hotspots(hotspots), condition(true)
{
    for (auto const& pose : poses) {
        duration += pose.duration;
    }
}

hotspot animation::hotspot_at(math::vec2 offset) {
    for (auto const& hotspot : hotspots) {
        if (hotspot.point_inside(offset)) {
            return hotspot;
        }
    }
    return {};
}

}
