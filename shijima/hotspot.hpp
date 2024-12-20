#pragma once
#include <string>
#include "math.hpp"

namespace shijima {

class hotspot {
public:
    enum class shape {
        INVALID = 0,
        ELLIPSE = 1,
        SHAPE_MAX = 2,
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