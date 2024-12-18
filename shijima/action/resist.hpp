#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class resist : public animate {
public:
    virtual bool tick() override;
};

}
}