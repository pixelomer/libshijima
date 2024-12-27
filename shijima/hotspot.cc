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
            return (std::pow((point.x - m_origin.x) / m_size.x, 2) +
                std::pow((point.y - m_origin.y) / m_size.y, 2)) < 1;
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