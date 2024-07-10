#pragma once
#include "sequence.hpp"

namespace shijima {
namespace action {

class select : public sequence {
protected:
    bool did_execute = false;
    virtual std::shared_ptr<base> next_action() override {
        if (did_execute) {
            if (action != nullptr) {
                action->finalize();
            }
            action = nullptr;
            return nullptr;
        }
        auto ret = sequence::next_action(); 
        did_execute = true;
        return ret;
    }
public:
    virtual void init(mascot::tick &ctx) override {
        did_execute = false;
        mascot::tick overlay_ctx = ctx.overlay({ { "Loops", "false "} });
        sequence::init(overlay_ctx);
    }
};

}
}