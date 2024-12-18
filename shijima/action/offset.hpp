#pragma once
#include "instant.hpp"

namespace shijima {
namespace action {

class offset : public instant {
public:
    virtual bool tick() override;
};

}
}