#include "reference.hpp"

namespace shijima {
namespace action {

bool reference::requests_vars() {
    return false;
}

void reference::init(mascot::tick &ctx) {
    base::init(ctx);
    mascot::tick target_ctx = ctx.overlay(init_attr);
    try {
        target->init(target_ctx);
    }
    catch (...) {
        base::finalize();
        throw;
    }
}

bool reference::tick() {
    if (!base::tick()) {
        return false;
    }
    return target->tick();
}

void reference::finalize() {
    base::finalize();
    target->finalize();
}

}
}