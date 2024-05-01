#pragma once
#include "state.hpp"
#include <shijima/scripting/context.hpp>
#include <memory>

namespace shijima {
namespace mascot {

class mascot {
public:
    std::shared_ptr<shijima::mascot::state> state;
    mascot() {
        state = std::make_shared<shijima::mascot::state>();
    }

};

}
}