#include "animate.hpp"

namespace shijima {
namespace action {

class transform : public animate {
public:
    virtual bool tick() override {
        bool ret = animate::tick();
        if (animation_finished()) {
            // Animation concluded, create a breed request
            // It is up to the library consumer to comply with the request

            auto &request = mascot->breed_request;
            request.available = true;
            request.behavior = vars.get_string("TransformBehavior", "Fall");
            request.name = vars.get_string("TransformMascot", "");
            request.transient = false; //FIXME: should this be true?
            
            request.anchor = { mascot->anchor.x, mascot->anchor.y };

            // This is a transformation so this shimeji should disappear now
            mascot->dead = true;

            return false;
        }
        return ret;
    }
};

}
}