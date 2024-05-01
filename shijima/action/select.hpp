#pragma once
#include "sequence.hpp"

namespace shijima {
namespace action {

class select : public sequence {
public:
    virtual void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& extra)
    {
        sequence::init(mascot, extra);
        vars.add_attr({{ "Loops", "false" }});
    }
};

}
}