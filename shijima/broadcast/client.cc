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

#include "client.hpp"
#include <stdexcept>

namespace shijima {
namespace broadcast {

client::client(std::shared_ptr<server_state> server): m_server(server) {}
client::client(): m_server(nullptr) {}

void client::finalize() {
    if (m_server != nullptr) {
        m_server->set_available(true);
    }
    m_server = nullptr;
}

bool client::connected() {
    return m_server != nullptr && m_server->active();
}

void client::notify_arrival() {
    if (!connected()) {
        throw std::runtime_error("notify_arrival() called on disconnected client");
    }
    m_server->notify_arrival();
}

bool client::did_meet_up() {
    return m_server != nullptr && m_server->did_meet_up();
}

math::vec2 client::get_target() {
    if (!connected()) {
        throw std::runtime_error("get_target() called on disconnected client");
    }
    return m_server->anchor;
}

interaction client::get_interaction() {
    if (!did_meet_up()) {
        throw std::runtime_error("get_interaction() called before meetup");
    }
    return { m_server->get_ongoing_pt(), m_server->client_behavior };
}

client &client::operator=(client &&rhs) {
    finalize();
    m_server = rhs.m_server;
    rhs.m_server = nullptr;
    return *this;
}

client::~client() {
    finalize();
}

}
}
