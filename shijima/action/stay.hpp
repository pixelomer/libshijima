#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class stay : public animation {
public:
    virtual bool tick() override;
};

}
}