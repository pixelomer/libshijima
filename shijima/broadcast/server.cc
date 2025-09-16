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

#include "server.hpp"
#include <stdexcept>

namespace shijima {
namespace broadcast {

bool server::active() {
    return m_state != nullptr && m_state->active();
}
bool server::available() {
    return m_state != nullptr && !m_state->did_meet_up()
        && m_state->available();
}
bool server::did_meet_up() {
    return m_state != nullptr && m_state->did_meet_up();
}
bool server::turn_requested() {
    return m_state != nullptr && m_state->turn_requested();
}
bool server::requested_looking_right() {
    return m_state != nullptr && m_state->requested_looking_right();
}
std::string server::affordance() {
    if (m_state != nullptr) {
        return m_state->affordance;
    }
    return "";
}
server::server() {}
server::server(math::vec2 anchor, std::string const& affordance):
    m_state(std::make_shared<server_state>())
{
    m_state->affordance = affordance;
    update_anchor(anchor);
}
void server::update_anchor(math::vec2 anchor) {
    if (!active()) {
        throw std::runtime_error("update_anchor() called on inactive server");
    }
    m_state->anchor = anchor;
}
math::vec2 server::get_anchor() {
    return m_state->anchor;
}
void server::finalize() {
    if (m_state != nullptr) {
        m_state->finalize();
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BROADCASTS, "Broadcast ended");
        #endif
    }
    m_state = nullptr;
}
client server::connect(std::string const& client_behavior,
    std::string const& server_behavior)
{
    if (!available()) {
        throw std::runtime_error("cannot connect to unavailable server");
    }
    m_state->client_behavior = client_behavior;
    m_state->server_behavior = server_behavior;
    m_state->set_available(false);
    return { m_state };
}
interaction server::get_interaction() {
    if (!did_meet_up()) {
        throw std::runtime_error("get_interaction() called before meetup");
    }
    return { m_state->get_ongoing_pt(), m_state->server_behavior };
}

}
}
