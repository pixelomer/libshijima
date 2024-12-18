#pragma once
#include "move.hpp"

namespace shijima {
namespace action {

class scanmove : public move {
public:
    virtual bool requests_broadcast() override;
    virtual void init(mascot::tick &ctx) override;
    virtual bool tick() override;
};

}
}