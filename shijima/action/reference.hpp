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

#include "base.hpp"
#include <memory>

namespace shijima {
namespace action {

class reference : public base {
public:
    std::shared_ptr<base> target;
    virtual bool requests_interpolation() override;
    virtual bool requests_vars() override;
    virtual void init(mascot::tick &ctx) override;
    virtual bool subtick(int idx) override;
    virtual void finalize() override;

    template<class Archive>
    void serialize(Archive &ar) {
        ar(init_attr, target);
    }
};

}
}