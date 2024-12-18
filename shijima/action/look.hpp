#pragma once
#include "instant.hpp"

namespace shijima {
namespace action {

class look : public instant {
public:
    virtual bool tick() override;
};

}
}