#pragma once
#include <memory>
#include <string>

namespace shijima {
namespace broadcast {

class interaction {
private:
    std::shared_ptr<bool> m_ongoing;
    std::string m_behavior;
public:
    bool started;
    interaction(std::shared_ptr<bool> ongoing_pt, std::string const& behavior):
        m_ongoing(ongoing_pt), m_behavior(behavior), started(false) {}
    interaction() {}
    std::string const& behavior() {
        return m_behavior;
    }
    bool ongoing() {
        return m_ongoing != nullptr && *m_ongoing;
    }
    bool available() {
        return m_ongoing != nullptr;
    }
    void finalize() {
        if (m_ongoing != nullptr) {
            *m_ongoing = false;
        }
        m_ongoing = nullptr;
    }
};

}
}