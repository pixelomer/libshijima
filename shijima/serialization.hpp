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

#include "config.hpp"
#include "action/action.hpp"
#include <cereal/archives/portable_binary.hpp>

// these macros break clangd
#ifndef __CLANGD__

// register types for serialization/deserialization
CEREAL_REGISTER_TYPE(shijima::action::animate);
CEREAL_REGISTER_TYPE(shijima::action::animation);
CEREAL_REGISTER_TYPE(shijima::action::base);
CEREAL_REGISTER_TYPE(shijima::action::breed);
CEREAL_REGISTER_TYPE(shijima::action::dragged);
CEREAL_REGISTER_TYPE(shijima::action::fall);
CEREAL_REGISTER_TYPE(shijima::action::interact);
CEREAL_REGISTER_TYPE(shijima::action::jump);
CEREAL_REGISTER_TYPE(shijima::action::look);
CEREAL_REGISTER_TYPE(shijima::action::move);
CEREAL_REGISTER_TYPE(shijima::action::movewithturn);
CEREAL_REGISTER_TYPE(shijima::action::offset);
CEREAL_REGISTER_TYPE(shijima::action::reference);
CEREAL_REGISTER_TYPE(shijima::action::resist);
CEREAL_REGISTER_TYPE(shijima::action::scanmove);
CEREAL_REGISTER_TYPE(shijima::action::select);
CEREAL_REGISTER_TYPE(shijima::action::selfdestruct);
CEREAL_REGISTER_TYPE(shijima::action::sequence);
CEREAL_REGISTER_TYPE(shijima::action::stay);
CEREAL_REGISTER_TYPE(shijima::action::transform);
CEREAL_REGISTER_TYPE(shijima::action::turn);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation, shijima::action::animate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::base,      shijima::action::animation);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,   shijima::action::breed);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation, shijima::action::dragged);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation, shijima::action::fall);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::base,      shijima::action::instant);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,   shijima::action::interact);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation, shijima::action::jump);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::instant,   shijima::action::look);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation, shijima::action::move);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::move,      shijima::action::movewithturn);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::instant,   shijima::action::offset);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::base,      shijima::action::reference);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,   shijima::action::resist);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::move,      shijima::action::scanmove);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::sequence,  shijima::action::select);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,   shijima::action::selfdestruct);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::base,      shijima::action::sequence);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation, shijima::action::stay);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,   shijima::action::transform);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,   shijima::action::turn);

#endif
