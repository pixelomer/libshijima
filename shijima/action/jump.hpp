#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class jump : public animation {
public:
    virtual bool tick() override;
};

}
}