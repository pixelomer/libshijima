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

#include "move.hpp"

namespace shijima {
namespace action {

class movewithturn : public move {
protected:
    bool is_turning = false;
    bool headed_right();
    bool needs_turn();
    virtual std::shared_ptr<shijima::animation> &get_animation() override;
public:
    virtual void init(mascot::tick &ctx) override;
};

}
}