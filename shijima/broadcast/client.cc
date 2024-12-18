#include "client.hpp"

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