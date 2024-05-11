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
    virtual void init(scripting::context &script_ctx,
        std::map<std::string, std::string> const& extra) override
    {
        base::init(script_ctx, extra);
        target->init(script_ctx, init_attr);
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