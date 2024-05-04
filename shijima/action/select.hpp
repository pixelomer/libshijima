#pragma once
#include "sequence.hpp"

namespace shijima {
namespace action {

class select : public sequence {
protected:
    bool did_execute = false;
    virtual std::shared_ptr<base> next_action() override {
        if (did_execute) {
            if (action != nullptr) {
                action->finalize();
            }
            action = nullptr;
            return nullptr;
        }
        auto ret = sequence::next_action(); 
        did_execute = true;
        return ret;
    }
public:
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra) override
    {
        did_execute = false;
        std::map<std::string, std::string> extra_copy = extra;
        extra_copy["Loops"] = "false";
        sequence::init(mascot, extra_copy);
    }
};

}
}