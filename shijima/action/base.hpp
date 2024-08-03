#pragma once
#include <shijima/scripting/variables.hpp>
#include <shijima/mascot/tick.hpp>
#include <shijima/broadcast/client.hpp>
#include <shijima/broadcast/server.hpp>
#include <shijima/log.hpp>

namespace shijima {
namespace action {

class base {
private:
    bool active = false;
    bool prevents_dragging;
protected:
    std::shared_ptr<mascot::state> mascot;
    scripting::variables vars;
    broadcast::server server;
    broadcast::client client;
    int start_time;

    void reset_elapsed() {
        start_time = mascot->time;
    }
    int elapsed() {
        return mascot->time - start_time;
    }
    template<typename T>
    T dx(T dx) {
        return (mascot->looking_right ? -1 : 1) * dx;
    }
    template<typename T>
    T dy(T dy) {
        return dy;
    }
public:
    virtual bool requests_vars() {
        return true;
    }
    virtual bool requests_broadcast() {
        return false;
    }

    std::map<std::string, std::string> init_attr;

    // Initializes the action, resetting any internal state as necessary.
    // If init() throws an exception, the object should be inactive afterwards.
    virtual void init(mascot::tick &ctx) {
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
        prevents_dragging = false;
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
            if (!draggable) {
                mascot->drag_lock++;
                prevents_dragging = true;
            }
            if (requests_broadcast()) {
                auto affordance = vars.get_string("Affordance");
                if (affordance != "") {
                    server = mascot->env->broadcasts.start_broadcast(
                        vars.get_string("Affordance"), mascot->anchor);
                    vars.add_attr({ { "Affordance", "" } });
                }
            }
        }
    }

    // Returns false if execution should immediately advance to the
    // next action. The action should return true for the last frame
    // and return false for the frame after the last.
    virtual bool tick() {
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
    virtual void finalize() {
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
        long drag_lock = mascot->drag_lock;
        if (prevents_dragging) {
            mascot->drag_lock = drag_lock -= 1;
        }
        mascot = nullptr;
        active = false;
        if (drag_lock < 0) {
            throw std::runtime_error("drag_lock went below zero");
        }
    }
    virtual ~base() {}
};

}
}