#include "reference.hpp"

namespace shijima {
namespace action {

bool reference::requests_vars() {
    return false;
}

bool reference::requests_interpolation() {
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

bool reference::subtick(int idx) {
    if (!base::subtick(idx)) {
        return false;
    }
    return target->subtick(idx);
}

void reference::finalize() {
    base::finalize();
    target->finalize();
}

}
}