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
#include <memory>
#include <shijima/math.hpp>

namespace shijima {
namespace broadcast {

class server_state {
private:
    bool m_finalized = false;
    bool m_available = true;
    bool m_met_up = false;
    std::shared_ptr<bool> m_ongoing_pt;
public:
    math::vec2 anchor;
    std::string client_behavior;
    std::string server_behavior;
    bool did_meet_up() { return m_met_up; }
    void notify_arrival() { m_met_up = true; finalize(); }
    bool active() { return !m_finalized; }
    bool available() { return active() && m_available; }
    void finalize() { m_finalized = true; }
    void set_available(bool available) { m_available = available; }
    std::shared_ptr<bool> get_ongoing_pt() {
        if (m_ongoing_pt == nullptr) {
            m_ongoing_pt = std::make_shared<bool>(true);
        }
        return m_ongoing_pt;
    }
};

}
}
