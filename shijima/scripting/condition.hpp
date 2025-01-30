#pragma once
#include "context.hpp"

namespace shijima {
namespace scripting {

class condition {
private:
    bool is_constant;
    bool value;
    std::string js;
    void init(std::string const& str) {
        if (str.size() > 3 && (str[0] == '$' || str[0] == '#') &&
            str[1] == '{' && str[str.size()-1] == '}')
        {
            is_constant = false;
            js = str.substr(2, str.size()-3);
        }
        else {
            is_constant = true;
            value = (str == "true");
        }
    }
    void init(bool val) {
        this->is_constant = true;
        this->value = val;
    }
public:
    condition(std::string const& str) {
        init(str);
    }
    condition(const char *str) {
        init(std::string(str));
    }
    condition(bool value) {
        init(value);
    }
    bool eval(context &ctx) const {
        if (is_constant) {
            return value;
        }
        return ctx.eval_bool(js);
    }
    bool eval(context::global::active &ctx) const {
        if (is_constant) {
            return value;
        }
        return ctx->eval_bool(js);
    }
};

}
}
