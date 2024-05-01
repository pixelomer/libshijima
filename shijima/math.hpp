#pragma once
#include <string>

namespace shijima {
namespace math {

struct rec {
    double x, y, width, height;
    rec() {}
    rec(double x, double y, double width, double height):
        x(x), y(y), width(width), height(height) {}
};

struct vec2 {
    double x, y;
    vec2() {}
    vec2(double x, double y): x(x), y(y) {}
    vec2(std::string const& str) {
        auto sep = str.find(',');
        if (sep == std::string::npos) {
            throw std::invalid_argument("missing separator");
        }
        x = std::stod(str.substr(0, sep));
        y = std::stod(str.substr(sep+1));
    }
};

}
}