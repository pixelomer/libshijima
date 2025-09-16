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
#include "client.hpp"
#include <shijima/log.hpp>
#include "server_state.hpp"
#include <shijima/broadcast/interaction.hpp>

namespace shijima {
namespace broadcast {

class server {
private:
    std::shared_ptr<server_state> m_state;
public:
    bool active();
    bool available();
    bool turn_requested();
    bool requested_looking_right();
    bool did_meet_up();
    std::string affordance();
    server();
    server(math::vec2 anchor, std::string const& affordance);
    void update_anchor(math::vec2 anchor);
    math::vec2 get_anchor();
    void finalize();
    client connect(std::string const& client_behavior,
        std::string const& server_behavior);
    interaction get_interaction();
};

}
}
