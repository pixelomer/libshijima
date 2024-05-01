#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class dragged : public animation {
public:
    virtual bool tick() {
        //FIXME: implement FootX, FootDX
        vars.add_attr({{ "FootX", "0" }, { "FootDX", "0" }});
        if (!animation::tick()) {
            return false;
        }
        return true;
    }
    virtual void finalize() {
        animation::finalize();
    }
};

}
}