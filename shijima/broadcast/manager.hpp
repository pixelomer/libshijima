#pragma once
#include "client.hpp"
#include "server.hpp"
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <shijima/log.hpp>

//FIXME: This implementation does not scale well

namespace shijima {
namespace broadcast {

class manager {
private:
    std::map<std::string, std::vector<server>> m_servers;
public:
    server start_broadcast(std::string const& affordance, math::vec2 anchor) {
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BROADCASTS, "Broadcasting affordance: " + affordance);
        #endif
        server new_server { anchor };
        m_servers[affordance].push_back(new_server);
        return new_server;
    }
    bool try_connect(client &peer, double y, std::string const& affordance,
        std::string const& client_behavior, std::string const& server_behavior)
    {
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BROADCASTS, "Trying to connect: " + affordance);
        #endif
        auto &servers = m_servers[affordance];
        for (long i=0; i<(long)servers.size(); ++i) {
            auto &server = servers[i];
            if (!server.active()) {
                servers.erase(servers.begin() + i);
                --i;
                continue;
            }
            if (std::fabs(y - server.get_anchor().y) > 1) {
                continue;
            }
            if (server.available()) {
                peer = server.connect(client_behavior, server_behavior);
                #ifdef SHIJIMA_LOGGING_ENABLED
                    log(SHIJIMA_LOG_BROADCASTS, "Connected: " + affordance);
                #endif
                return true;
            }
        }
        return false;
    }
};

}
}