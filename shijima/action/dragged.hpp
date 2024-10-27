#pragma once
#include "animation.hpp"

// "footX" is required for backwards compatibility

namespace shijima {
namespace action {

class dragged : public animation {
protected:
    double foot_x;
    double foot_dx;
    int time_to_resist;
    math::vec2 dcursor;
public:
    virtual void init(mascot::tick &ctx) override {
        animation::init(ctx);
        auto offset_x = vars.get_num("OffsetX", 0);
        foot_dx = 0;
        foot_x = mascot->get_cursor().x + offset_x;
        time_to_resist = 250;
        vars.add_attr({ { "FootX", foot_x }, { "footX", foot_x } });
        mascot->reset_dcursor_buffer();
        dcursor = {};
    }
    virtual bool tick() override {
        mascot->looking_right = false;
        auto cursor = mascot->get_cursor();
        dcursor.x += cursor.dx / mascot->env->subticks_per_tick;
        dcursor.y += cursor.dy / mascot->env->subticks_per_tick;
        if (mascot->new_tick()) {
            mascot->push_dcursor(dcursor);
            dcursor = {};
            foot_dx = (foot_dx + ((cursor.x - foot_x) * 0.1)) * 0.8;
            foot_x += foot_dx;
            vars.add_attr({ { "FootX", foot_x }, { "footX", foot_x },
                { "FootDX", foot_dx }, { "footDX", foot_dx } });
            auto offset_x = vars.get_num("OffsetX", 0);
            auto offset_y = vars.get_num("OffsetY", 120);
            if (std::fabs(cursor.x - mascot->anchor.x + offset_x) >= 5.0 * dt()) {
                reset_elapsed();
            }
            if (elapsed() >= time_to_resist) {
                return false;
            }
            mascot->anchor.x = cursor.x + offset_x;
            mascot->anchor.y = cursor.y + offset_y;
        }
        else {
            mascot->anchor.x += cursor.dx / mascot->env->subticks_per_tick;
            mascot->anchor.y += cursor.dy / mascot->env->subticks_per_tick;
        }
        return animation::tick();
    }
};

}
}