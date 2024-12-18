#pragma once
#include "sequence.hpp"

namespace shijima {
namespace action {

class select : public sequence {
protected:
    bool did_execute = false;
    virtual std::shared_ptr<base> next_action() override;
public:
    virtual void init(mascot::tick &ctx) override;
};

}
}