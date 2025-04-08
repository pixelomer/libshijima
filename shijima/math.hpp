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

    template<class Archive>
    void serialize(Archive &ar) {
        ar(x, y, width, height);
    }
};

struct vec2 {
    double x, y;
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
    bool operator==(vec2 const& rhs) {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(vec2 const& rhs) {
        return !(*this == rhs);
    }
    vec2 operator*(double rhs) {
        return { x * rhs, y * rhs };
    }
    vec2 &operator*=(double rhs) {
        return *this = *this * rhs;
    }
    vec2 operator/(double rhs) {
        return { x / rhs, y / rhs };
    }
    vec2 &operator/=(double rhs) {
        return *this = *this / rhs;
    }
    vec2 operator-(vec2 const& rhs) {
        return { x - rhs.x, y - rhs.y };
    }
    vec2 &operator-=(vec2 const& rhs) {
        return *this = *this - rhs;
    }
    vec2 operator+(vec2 const& rhs) {
        return { x + rhs.x, y + rhs.y };
    }
    vec2 &operator+=(vec2 const& rhs) {
        return *this = *this + rhs;
    }

    template<class Archive>
    void serialize(Archive &ar) {
        ar(x, y);
    }
};

}
}
