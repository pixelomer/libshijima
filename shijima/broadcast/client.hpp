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
#include "interaction.hpp"
#include "server_state.hpp"

namespace shijima {
namespace broadcast {

class client {
private:
    std::shared_ptr<server_state> m_server;
public:
    void finalize();
    bool connected();
    client(std::shared_ptr<server_state> server);
    client();
    client(client const& other) = delete;
    client(client &&other) = delete;
    client &operator=(client const& rhs) = delete;
    client &operator=(client &&rhs);
    ~client();
    void notify_arrival();
    bool did_meet_up();
    math::vec2 get_target();
    interaction get_interaction();
};

}
}
