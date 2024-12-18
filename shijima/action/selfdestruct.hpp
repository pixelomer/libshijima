#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class selfdestruct : public animate {
public:
    virtual bool tick() override;
};

}
}