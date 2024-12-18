#pragma once
#include "base.hpp"
#include <memory>

namespace shijima {
namespace action {

class reference : public base {
public:
    std::shared_ptr<base> target;
    virtual bool requests_vars() override;
    virtual void init(mascot::tick &ctx) override;
    virtual bool tick() override;
    virtual void finalize() override;
};

}
}