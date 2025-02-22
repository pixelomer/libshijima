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

#include <vector>
#include "scripting/condition.hpp"
#include "pose.hpp"
#include "hotspot.hpp"

namespace shijima {

class animation {
private:
    std::vector<pose> poses;
    std::vector<hotspot> hotspots;
    int duration = 0;
public:
    scripting::condition condition;
    // time is 0-indexed. The first frame happens at t=0
    pose const& get_pose(int time);
    hotspot hotspot_at(math::vec2 offset);
    int get_duration();
    animation(std::vector<shijima::pose> const& poses,
        std::vector<shijima::hotspot> const& hotspots);
};

}
