#include "animate.hpp"

namespace shijima {
namespace action {

class breed : public animate {
private:
    bool transient;
public:
    virtual bool tick() override {
        if (mascot->new_tick()) {
            transient = vars.get_bool("BornTransient", false);
            if (!transient && (!mascot->env->allows_breeding || !mascot->can_breed)) {
                return false;
            }
        }
        bool ret = animate::tick();
        if (mascot->new_tick() && animation_finished()) {
            // Animation concluded, create a breed request
            // It is up to the library consumer to comply with the request

            auto &request = mascot->breed_request;
            double born_x = vars.get_num("BornX", 0);
            double born_y = vars.get_num("BornY", 0);
            request.available = true;
            request.behavior = vars.get_string("BornBehavior", "Fall");
            request.name = vars.get_string("BornMascot", "");
            request.transient = transient;
            
            request.anchor = { mascot->anchor.x + dx(born_x),
                mascot->anchor.y + dy(born_y) };

            return false;
        }
        return ret;
    }
};

}
}