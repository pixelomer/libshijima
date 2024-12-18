#include "select.hpp"

namespace shijima {
namespace action {

std::shared_ptr<base> select::next_action() {
    if (did_execute) {
        if (action != nullptr) {
            action->finalize();
        }
        action = nullptr;
        return nullptr;
    }
    auto ret = sequence::next_action(); 
    did_execute = true;
    return ret;
}

void select::init(mascot::tick &ctx) {
    did_execute = false;
    mascot::tick overlay_ctx = ctx.overlay({ { "Loops", "false "} });
    sequence::init(overlay_ctx);
}

}
}