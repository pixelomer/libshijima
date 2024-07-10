#pragma once
#include "base.hpp"
#include <memory>

namespace shijima {
namespace action {

class reference : public base {
public:
    std::shared_ptr<base> target;
    virtual bool requests_vars() override {
        return false;
    }
    virtual void init(mascot::tick &ctx) override {
        base::init(ctx);
        mascot::tick target_ctx = ctx.overlay(init_attr);
        target->init(target_ctx);
    }
    virtual bool tick() override {
        if (!base::tick()) {
            return false;
        }
        return target->tick();
    }
    virtual void finalize() override {
        base::finalize();
        target->finalize();
    }
};

}
}