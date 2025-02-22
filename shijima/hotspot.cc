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

#include "hotspot.hpp"
#include <cmath>

namespace shijima {

hotspot::shape hotspot::shape_from_name(std::string const& name) {
    if (name == "Ellipse") {
        return shape::ELLIPSE;
    }
    else if (name == "Rectangle") {
        return shape::RECTANGLE;
    }
    else {
        return shape::INVALID;
    }
}

hotspot::shape hotspot::get_shape() const {
    return m_shape;
}

std::string const& hotspot::get_behavior() const {
    return m_behavior;
}

bool hotspot::point_inside(math::vec2 point) const {
    switch (m_shape) {
        case shape::ELLIPSE: {
            return (std::pow((point.x - (m_origin.x + m_size.x / 2)) / m_size.x, 2) +
                std::pow((point.y - (m_origin.y + m_size.y / 2)) / m_size.y, 2)) < 1;
        }
        case shape::RECTANGLE: {
            return (point.x >= m_origin.x) && (point.x <= (m_origin.x + m_size.x)) &&
                (point.y >= m_origin.y) && (point.y <= (m_origin.y + m_size.y));
        }
        default:
            return false;
    }
}

hotspot::hotspot(hotspot::shape shape, math::vec2 origin,
    math::vec2 size, std::string const& behavior):
    m_shape(shape), m_origin(origin),
    m_size(size), m_behavior(behavior)
{
    if (m_shape < shape::INVALID || m_shape > shape::SHAPE_MAX) {
        m_shape = shape::INVALID;
    }
}

bool hotspot::valid() const {
    return m_shape != shape::INVALID;
}

hotspot::hotspot(): m_shape(shape::INVALID), m_behavior("") {}

}
