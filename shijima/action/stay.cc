#include "stay.hpp"

namespace shijima {
namespace action {

bool stay::tick() {
    if (!animation::tick()) {
        return false;
    }
    return elapsed() < vars.get_num("Duration", 250);
}

}
}