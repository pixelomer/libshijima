#pragma once
#include "animation.hpp"

namespace shijima {
namespace action {

class dragged : public animation {
protected:
    double foot_x;
    double foot_dx;
    int time_to_resist;
public:
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra)
    {
        animation::init(mascot, extra);
        auto offset_x = vars.get_num("OffsetX", 0);
        foot_dx = 0;
        foot_x = mascot->env.cursor.x + offset_x;
        time_to_resist = 250;
        vars.add_attr({{ "FootX", foot_x }});
    }
    virtual bool tick() {
        mascot->looking_right = false;
        auto cursor = mascot->env.cursor;
        foot_dx = (foot_dx + ((cursor.x - foot_x) * 0.1)) * 0.8;
        foot_x += foot_dx;
        vars.add_attr({ { "FootX", foot_x }, { "FootDX", foot_dx } });
        auto offset_x = vars.get_num("OffsetX", 0);
        auto offset_y = vars.get_num("OffsetY", 120);
        if (std::abs(cursor.x - mascot->anchor.x + offset_x) >= 5) {
            reset_elapsed();
        }
        if (!animation::tick()) {
            return false;
        }
        mascot->anchor.x = cursor.x + offset_x;
        mascot->anchor.y = cursor.y + offset_y;
        if (elapsed() >= time_to_resist) {
            return false;
        }
        return true;
    }
    virtual void finalize() {
        animation::finalize();
    }
};

}
}