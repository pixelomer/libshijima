#pragma once
#include "base.hpp"
#include <memory>

namespace shijima {
namespace action {

class reference : public base {
public:
    std::shared_ptr<base> target;
    virtual bool prevents_dragging() override;
    virtual bool requests_interpolation() override;
    virtual bool requests_vars() override;
    virtual void init(mascot::tick &ctx) override;
    virtual bool subtick(int idx) override;
    virtual void finalize() override;
};

}
}