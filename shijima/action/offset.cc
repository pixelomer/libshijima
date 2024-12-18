#include "offset.hpp"

namespace shijima {
namespace action {

bool offset::tick() {
    mascot->anchor.x += (int)vars.get_num("X", 0);
    mascot->anchor.y += (int)vars.get_num("Y", 0);
    return false;
}

}
}