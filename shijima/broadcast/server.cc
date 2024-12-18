#include "server.hpp"

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
server::server() {}
server::server(math::vec2 anchor): m_state(std::make_shared<server_state>()) {
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