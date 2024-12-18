#include "interaction.hpp"

namespace shijima {
namespace broadcast {

interaction::interaction(std::shared_ptr<bool> ongoing_pt, std::string const& behavior):
    m_ongoing(ongoing_pt), m_behavior(behavior), started(false) {}
interaction::interaction() {}

std::string const& interaction::behavior() {
    return m_behavior;
}

bool interaction::ongoing() {
    return m_ongoing != nullptr && *m_ongoing;
}

bool interaction::available() {
    return m_ongoing != nullptr;
}

void interaction::finalize() {
    if (m_ongoing != nullptr) {
        *m_ongoing = false;
    }
    m_ongoing = nullptr;
}

}
}