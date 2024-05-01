#pragma once
#include "base.hpp"
#include <memory>

namespace shijima {
namespace action {

class reference : public base {
public:
    std::shared_ptr<base> target;
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra)
    {
        base::init(mascot, extra);
        target->init(mascot, init_attr);
    }
    virtual bool tick() {
        if (!base::tick()) {
            return false;
        }
        return target->tick();
    }
    virtual void finalize() {
        base::finalize();
        target->finalize();
    }
};

}
}