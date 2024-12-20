#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class resist : public animate {
protected:
    virtual bool handle_dragging() override;
public:
    virtual bool tick() override;
};

}
}