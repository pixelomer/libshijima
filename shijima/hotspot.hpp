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

#include <string>
#include "math.hpp"

namespace shijima {

class hotspot {
public:
    enum class shape {
        INVALID = 0,
        ELLIPSE = 1,
        RECTANGLE = 2,
        SHAPE_MAX = 3,
    };
private:
    shape m_shape;
    math::vec2 m_origin;
    math::vec2 m_size;
    std::string m_behavior;
public:
    static shape shape_from_name(std::string const& name);
    shape get_shape() const;
    std::string const& get_behavior() const;
    bool point_inside(math::vec2 point) const;
    bool valid() const;
    hotspot();
    hotspot(hotspot::shape shape, math::vec2 origin,
        math::vec2 size, std::string const& behavior);
};

}
