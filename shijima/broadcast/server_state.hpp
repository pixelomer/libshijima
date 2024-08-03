#pragma once
#include <string>
#include <memory>
#include <stdexcept>
#include <shijima/math.hpp>
#include "client.hpp"

namespace shijima {
namespace broadcast {

class server_state {
private:
    bool m_finalized = false;
    bool m_available = true;
    bool m_met_up = false;
    std::shared_ptr<bool> m_ongoing_pt;
public:
    math::vec2 anchor;
    std::string client_behavior;
    std::string server_behavior;
    bool did_meet_up() { return m_met_up; }
    void notify_arrival() { m_met_up = true; finalize(); }
    bool active() { return !m_finalized; }
    bool available() { return active() && m_available; }
    void finalize() { m_finalized = true; }
    void set_available(bool available) { m_available = available; }
    std::shared_ptr<bool> get_ongoing_pt() {
        if (m_ongoing_pt == nullptr) {
            m_ongoing_pt = std::make_shared<bool>(true);
        }
        return m_ongoing_pt;
    }
};

}
}