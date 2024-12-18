#pragma once
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