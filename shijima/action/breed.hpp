#include "animate.hpp"

namespace shijima {
namespace action {

class breed : public animate {
public:
    virtual bool tick() override {
        bool ret = animate::tick();
        if (animation_finished()) {
            // Animation concluded, create a breed request
            // It is up to the library consumer to comply with the request

            auto &request = mascot->breed_request;
            double born_x = vars.get_num("BornX", 0);
            double born_y = vars.get_num("BornY", 0);
            request.available = true;
            request.behavior = vars.get_string("BornBehavior", "Fall");
            request.name = vars.get_string("BornMascot", "");
            request.transient = vars.get_bool("BornTransient", false);
            
            request.anchor = { mascot->anchor.x + dx(born_x),
                mascot->anchor.y + dy(born_y) };

            return false;
        }
        return ret;
    }
};

}
}