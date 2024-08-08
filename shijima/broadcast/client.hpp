#pragma once
#include <string>
#include <memory>
#include "interaction.hpp"
#include "server_state.hpp"

namespace shijima {
namespace broadcast {

class client {
private:
    std::shared_ptr<server_state> m_server;
public:
    void finalize() {
        if (m_server != nullptr) {
            m_server->set_available(true);
        }
        m_server = nullptr;
    }
    bool connected() {
        return m_server != nullptr && m_server->active();
    }
    client(std::shared_ptr<server_state> server): m_server(server) {}
    client(): m_server(nullptr) {}
    client(client const& other) = delete;
    client(client &&other) = delete;
    client &operator=(client const& rhs) = delete;
    client &operator=(client &&rhs) {
        finalize();
        m_server = rhs.m_server;
        rhs.m_server = nullptr;
        return *this;
    }
    ~client() {
        finalize();
    }
    void notify_arrival() {
        if (!connected()) {
            throw std::runtime_error("notify_arrival() called on disconnected client");
        }
        m_server->notify_arrival();
    }
    bool did_meet_up() {
        return m_server != nullptr && m_server->did_meet_up();
    }
    math::vec2 get_target() {
        if (!connected()) {
            throw std::runtime_error("get_target() called on disconnected client");
        }
        return m_server->anchor;
    }
    interaction get_interaction() {
        if (!did_meet_up()) {
            throw std::runtime_error("get_interaction() called before meetup");
        }
        return { m_server->get_ongoing_pt(), m_server->client_behavior };
    }
};

}
}