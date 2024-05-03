#pragma once
#include "context.hpp"

namespace shijima {
namespace scripting {

class condition {
private:
    bool constant;
    bool value;
    std::string js;
public:
    condition(std::string const& str) {
        if (str.size() > 3 && (str[0] == '$' || str[0] == '#') &&
            str[1] == '{' && str[str.size()-1] == '}')
        {
            constant = false;
            js = str.substr(2, str.size()-3);
        }
        else {
            constant = true;
            value = (str == "true");
        }
    }
    condition(bool value) {
        this->constant = true;
        this->value = value;
    }
    bool eval(context &ctx) {
        if (constant) {
            return value;
        }
        return ctx.eval_bool(js);
    }
};

}
}