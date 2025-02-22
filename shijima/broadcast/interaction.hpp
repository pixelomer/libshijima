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

#include <memory>
#include <string>

namespace shijima {
namespace broadcast {

class interaction {
private:
    std::shared_ptr<bool> m_ongoing;
    std::string m_behavior;
public:
    bool started;
    interaction(std::shared_ptr<bool> ongoing_pt, std::string const& behavior);
    interaction();
    std::string const& behavior();
    bool ongoing();
    bool available();
    void finalize();
};

}
}
