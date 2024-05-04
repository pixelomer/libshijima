#pragma once
#include <shijima/scripting/variables.hpp>

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
    int dx(int dx) {
        return (mascot->looking_right ? -1 : 1) * dx;
    }
    int dy(int dy) {
        return dy;
    }
public:
    std::map<std::string, std::string> init_attr;
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra)
    {
        if (init_attr.count("Name") == 1) {
            std::cout << "(action) " << init_attr.at("Name") <<
                "::init()" << std::endl;
        }
        else {
            std::cout << "(action) <type:" << init_attr.at("Type") <<
                ">::init()" << std::endl;
        }
        if (active) {
            throw std::logic_error("init() called twice");
        }
        active = true;
        start_time = mascot->time;
        this->mascot = mascot;
        std::map<std::string, std::string> attr = init_attr;
        for (auto const& pair : extra) {
            attr[pair.first] = pair.second;
        }
        vars.init(mascot, attr);
    }
    // Returns false if execution should immediately advance to the
    // next action. The action should return true for the last frame
    // and return false for the frame after the last.
    virtual bool tick() {
        vars.tick();
        if (!vars.get_bool("Condition", true)) {
            return false;
        }

        auto border_type = vars.get_string("BorderType");
        if (border_type == "Floor") {
            return mascot->env.floor.is_on(mascot->anchor) ||
                mascot->env.active_ie.top_border().is_on(mascot->anchor);
        }
        else if (border_type == "Wall") {
            return mascot->env.work_area.left_border().is_on(mascot->anchor) ||
                mascot->env.work_area.right_border().is_on(mascot->anchor) ||
                mascot->env.active_ie.left_border().is_on(mascot->anchor) ||
                mascot->env.active_ie.right_border().is_on(mascot->anchor);
        }
        else if (border_type == "Ceiling") {
            return mascot->env.work_area.top_border().is_on(mascot->anchor) ||
                mascot->env.active_ie.bottom_border().is_on(mascot->anchor);
        }
        else if (border_type == "") {
            return true;
        }
        else {
            throw std::logic_error("Unknown border: " + border_type);
        }
    }
    virtual void finalize() {
        if (init_attr.count("Name") == 1) {
            std::cout << "(action) " << init_attr.at("Name") <<
                "::finalize()" << std::endl;
        }
        else {
            std::cout << "(action) <type:" << init_attr.at("Type") <<
                ">::finalize()" << std::endl;
        }
        if (!active) {
            throw std::logic_error("finalize() called twice");
        }
        vars.finalize();
        active = false;
    }
    virtual ~base() {}
};

}
}