#include "look.hpp"

namespace shijima {
namespace action {

bool look::tick() {
    mascot->looking_right = vars.get_bool("LookRight", false);
    return false;
}

}
}