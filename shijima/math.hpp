#pragma once
#include <stdexcept>
#include <string>
#include <cmath>

namespace shijima {
namespace math {

struct rec {
    double x, y, width, height;
    rec() {}
    rec(double x, double y, double width, double height):
        x(x), y(y), width(width), height(height) {}
    rec operator*(double rhs) {
        return { x * rhs, y * rhs, width * rhs, height * rhs };
    }
    rec &operator*=(double rhs) {
        return *this = *this * rhs;
    }
    rec operator/(double rhs) {
        return { x / rhs, y / rhs, width / rhs, height / rhs };
    }
    rec &operator/=(double rhs) {
        return *this = *this / rhs;
    }
};

struct vec2 {
    double x, y;
    static const vec2 nan() {
        return { NAN, NAN };
    }
    bool isnan() const {
        return std::isnan(x) || std::isnan(y);
    }
    vec2(): x(0), y(0) {}
    vec2(double x, double y): x(x), y(y) {}
    vec2(std::string const& str) {
        auto sep = str.find(',');
        if (sep == std::string::npos) {
            //throw std::invalid_argument("missing separator");
            x = 0;
            y = 0;
        }
        else {
            try {
                x = std::stod(str.substr(0, sep));
                y = std::stod(str.substr(sep+1));
            }
            catch (...) {
                x = 0;
                y = 0;
            }
        }
    }
    static bool validate_str(std::string const& str) {
        auto sep = str.find(',');
        if (sep == std::string::npos) {
            return false;
        }
        else {
            try {
                std::stod(str.substr(0, sep));
                std::stod(str.substr(sep+1));
                return true;
            }
            catch (...) {
                return false;
            }
        }
    }
    bool operator==(vec2 const& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(vec2 const& rhs) const {
        return !(*this == rhs);
    }
    vec2 operator*(double rhs) const {
        return { x * rhs, y * rhs };
    }
    vec2 &operator*=(double rhs) {
        return *this = *this * rhs;
    }
    vec2 operator/(double rhs) const {
        return { x / rhs, y / rhs };
    }
    vec2 &operator/=(double rhs) {
        return *this = *this / rhs;
    }
    vec2 operator+(vec2 const& rhs) const {
        return { x + rhs.x, y + rhs.y };
    }
    vec2 &operator+=(vec2 const& rhs) {
        return *this = *this + rhs;
    }
    vec2 operator-(vec2 const& rhs) const {
        return { x - rhs.x, y - rhs.y };
    }
    vec2 &operator-=(vec2 const& rhs) {
        return *this = *this - rhs;
    }
};

}
}