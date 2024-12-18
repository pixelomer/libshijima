#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class interact : public animate {
public:
    virtual bool tick() override;
};

}
}