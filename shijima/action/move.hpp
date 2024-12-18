#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class move : public animation {
public:
    virtual bool tick();
};

}
}