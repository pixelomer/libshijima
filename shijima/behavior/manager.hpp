#pragma once
#include <vector>
#include <memory>
#include "base.hpp"

namespace shijima {
namespace behavior {

class manager {
private:
    std::vector<std::shared_ptr<base>> initial;
    std::vector<std::shared_ptr<base>> active;
public:
};

}
}