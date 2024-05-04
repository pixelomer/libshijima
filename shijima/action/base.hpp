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
        return true;
    }
    virtual void finalize() {
        vars.finalize();
        active = false;
    }
    virtual ~base() {}
};

}
}