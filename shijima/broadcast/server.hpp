#pragma once
#include <string>
#include <memory>
#include <stdexcept>
#include <shijima/log.hpp>
#include "server_state.hpp"
#include <shijima/broadcast/interaction.hpp>

namespace shijima {
namespace broadcast {

class server {
private:
    std::shared_ptr<server_state> m_state;
public:
    bool active() { return m_state != nullptr && m_state->active(); }
    bool available() { return m_state != nullptr && !m_state->did_meet_up()
        && m_state->available(); }
    bool did_meet_up() { return m_state != nullptr && m_state->did_meet_up(); }
    server() {}
    server(math::vec2 anchor): m_state(std::make_shared<server_state>()) {
        update_anchor(anchor);
    }
    void update_anchor(math::vec2 anchor) {
        if (!active()) {
            throw std::runtime_error("update_anchor() called on inactive server");
        }
        m_state->anchor = anchor;
    }
    math::vec2 get_anchor() {
        return m_state->anchor;
    }
    void finalize() {
        if (m_state != nullptr) {
            m_state->finalize();
            #ifdef SHIJIMA_LOGGING_ENABLED
                log(SHIJIMA_LOG_BROADCASTS, "Broadcast ended");
            #endif
        }
        m_state = nullptr;
    }
    client connect(std::string const& client_behavior,
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
    interaction get_interaction() {
        if (!did_meet_up()) {
            throw std::runtime_error("get_interaction() called before meetup");
        }
        return { m_state->get_ongoing_pt(), m_state->server_behavior };
    }
};

}
}