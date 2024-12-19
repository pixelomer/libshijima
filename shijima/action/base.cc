#include "base.hpp"
#include <stdexcept>

namespace shijima {
namespace action {

bool base::requests_vars() {
    return true;
}

bool base::requests_broadcast() {
    return false;
}

bool base::requests_interpolation() {
    return true;
}

bool base::prevents_dragging() {
    return m_prevents_dragging;
}

void base::init(mascot::tick &ctx) {
    ctx.will_init();
    #ifdef SHIJIMA_LOGGING_ENABLED
        if (get_log_level() & SHIJIMA_LOG_ACTIONS) {
            if (init_attr.count("Name") == 1) {
                log("(action) " + init_attr.at("Name") + "::init()");
            }
            else {
                log("(action) <type:" + init_attr.at("Type") + ">::init()");
            }
        }
    #endif
    if (active) {
        throw std::logic_error("init() called twice");
    }
    m_prevents_dragging = false;
    active = true;
    mascot = ctx.script->state;
    start_time = mascot->time;
    std::map<std::string, std::string> attr = init_attr;
    for (auto const& pair : ctx.extra_attr) {
        attr[pair.first] = pair.second;
    }
    if (requests_vars()) {
        vars.init(*ctx.script, attr);
        bool draggable = vars.get_bool("Draggable", true);
        m_prevents_dragging = !draggable;
        if (requests_broadcast()) {
            auto affordance = vars.get_string("Affordance");
            if (affordance != "") {
                server = mascot->env->broadcasts.start_broadcast(
                    vars.get_string("Affordance"), mascot->anchor);
                vars.add_attr({ { "Affordance", "" } });
            }
        }
    }
    else {
        m_prevents_dragging = false;
    }
}

bool base::tick() {
    if (!requests_vars()) {
        return true;
    }
    if (server.active()) {
        server.update_anchor(mascot->anchor);
    }
    if (server.did_meet_up()) {
        mascot->interaction = server.get_interaction();
        mascot->queued_behavior = mascot->interaction.behavior();
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_BROADCASTS, "Server did meet client, starting interaction");
            log(SHIJIMA_LOG_BROADCASTS, "Queued behavior: " + mascot->queued_behavior);
        #endif
        return true;
    }
    vars.tick();
    if (!vars.get_bool("Condition", true)) {
        return false;
    }

    if (mascot->dragging && !prevents_dragging()) {
        // Started dragging
        mascot->was_on_ie = false;
        mascot->interaction.finalize();
        mascot->queued_behavior = "Dragged";
        return false;
    }

    auto border_type = vars.get_string("BorderType");
    if (border_type == "Floor") {
        return mascot->env->floor.is_on(mascot->anchor) ||
            mascot->env->active_ie.top_border().is_on(mascot->anchor);
    }
    else if (border_type == "Wall") {
        return mascot->env->work_area.left_border().is_on(mascot->anchor) ||
            mascot->env->work_area.right_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.left_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.right_border().is_on(mascot->anchor);
    }
    else if (border_type == "Ceiling") {
        return mascot->env->work_area.top_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.bottom_border().is_on(mascot->anchor);
    }
    else if (border_type == "") {
        return true;
    }
    else {
        throw std::logic_error("Unknown border: " + border_type);
    }
}

bool base::subtick(int idx) {
    if (requests_interpolation()) {
        if (idx == 0) {
            start_anchor = mascot->anchor;
            if (!tick()) {
                return false;
            }
            target_offset = mascot->anchor - start_anchor;
        }
        mascot->anchor = start_anchor + target_offset *
            ((idx + 1) / (double)mascot->env->subtick_count);
        return true;
    }
    else {
        return (idx != 0) || tick();
    }
}

void base::finalize() {
    #ifdef SHIJIMA_LOGGING_ENABLED
        if (get_log_level() & SHIJIMA_LOG_ACTIONS) {
            if (init_attr.count("Name") == 1) {
                log("(action) " + init_attr.at("Name") + "::finalize()");
            }
            else {
                log("(action) <type:" + init_attr.at("Type") + ">::finalize()");
            }
        }
    #endif
    if (!active) {
        throw std::logic_error("finalize() called twice");
    }
    if (requests_vars()) {
        server.finalize();
        client.finalize();
        vars.finalize();
    }
    mascot = nullptr;
    active = false;
}

}
}