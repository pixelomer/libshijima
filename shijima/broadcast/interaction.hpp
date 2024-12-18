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
    interaction(std::shared_ptr<bool> ongoing_pt, std::string const& behavior);
    interaction();
    std::string const& behavior();
    bool ongoing();
    bool available();
    void finalize();
};

}
}