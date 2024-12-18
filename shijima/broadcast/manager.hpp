#pragma once
#include "client.hpp"
#include "server.hpp"
#include <string>
#include <map>
#include <vector>

namespace shijima {
namespace broadcast {

class manager {
private:
    std::map<std::string, std::vector<server>> m_servers;
public:
    server start_broadcast(std::string const& affordance, math::vec2 anchor);
    bool try_connect(client &peer, double y, std::string const& affordance,
        std::string const& client_behavior, std::string const& server_behavior);
};

}
}