#pragma once
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
    bool did_meet_up();
    server();
    server(math::vec2 anchor);
    void update_anchor(math::vec2 anchor);
    math::vec2 get_anchor();
    void finalize();
    client connect(std::string const& client_behavior,
        std::string const& server_behavior);
    interaction get_interaction();
};

}
}