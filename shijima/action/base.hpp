#pragma once
#include <shijima/scripting/variables.hpp>
#include <shijima/mascot/tick.hpp>
#include <shijima/log.hpp>

namespace shijima {
namespace action {

class base {
private:
    bool active = false;
protected:
    std::shared_ptr<mascot::state> mascot;
    scripting::variables vars;
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

    std::map<std::string, std::string> init_attr;
    virtual void init(mascot::tick &ctx) {
        ctx.will_init();
        if (get_log_level() & SHIJIMA_LOG_ACTIONS) {
            if (init_attr.count("Name") == 1) {
                log("(action) " + init_attr.at("Name") + "::init()");
            }
            else {
                log("(action) <type:" + init_attr.at("Type") + ">::init()");
            }
        }
        if (active) {
            throw std::logic_error("init() called twice");
        }
        active = true;
        mascot = ctx.script->state;
        start_time = mascot->time;
        std::map<std::string, std::string> attr = init_attr;
        for (auto const& pair : ctx.extra_attr) {
            attr[pair.first] = pair.second;
        }
        if (requests_vars()) {
            vars.init(*ctx.script, attr);
        }
    }
    // Returns false if execution should immediately advance to the
    // next action. The action should return true for the last frame
    // and return false for the frame after the last.
    virtual bool tick() {
        if (!requests_vars()) {
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
        if (get_log_level() & SHIJIMA_LOG_ACTIONS) {
            if (init_attr.count("Name") == 1) {
                log("(action) " + init_attr.at("Name") + "::finalize()");
            }
            else {
                log("(action) <type:" + init_attr.at("Type") + ">::finalize()");
            }
        }
        if (!active) {
            throw std::logic_error("finalize() called twice");
        }
        if (requests_vars()) {
            vars.finalize();
        }
        mascot = nullptr;
        active = false;
    }
    virtual ~base() {}
};

}
}