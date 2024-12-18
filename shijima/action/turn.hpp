#pragma once
#include "animate.hpp"

namespace shijima {
namespace action {

class turn : public animate {
public:
    virtual bool tick() override;
};

}
}