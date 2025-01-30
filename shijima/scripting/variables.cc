#include "duktape/duktape.h"
#include "variables.hpp"

namespace shijima {
namespace scripting {

signed char variables::dynamic_prefix(std::string const& str) {
    if (str.size() >= 3 && str[1] == '{' &&
        str[str.size()-1] == '}' &&
        (str[0] == '$' || str[0] == '#'))
    {
        return str[0];
    }
    return -1;
}
bool variables::is_num(std::string const& str) {
    char *end = NULL;
    double val = strtod(str.c_str(), &end);
    return end != str.c_str() && *end == '\0' && val != HUGE_VAL;
}

variables::variables() {}

void variables::add_attr(std::map<std::string, double> const& attr) {
    auto ctx = global.use();
    for (auto const& pair : attr) {
        auto &key = pair.first;
        auto val = pair.second;
        dynamic_attr.erase(key);
        attr_keys.insert(key);
        duk_push_number(ctx->duk, val);
        duk_put_global_string(ctx->duk, key.c_str());
    }
}
void variables::add_attr(std::map<std::string, std::string> const& attr) {
    auto ctx = global.use();
    for (auto const& pair : attr) {
        auto &key = pair.first;
        auto val = pair.second;
        if (val == "null" || val == "true" || val == "false") {
            val = "${" + val + "}";
        }
        dynamic_attr.erase(key);
        attr_keys.insert(key);
        auto c = dynamic_prefix(val);
        switch (c) {
            case -1:
                // static
                if (is_num(val)) {
                    duk_push_number(ctx->duk, std::stod(val));
                }
                else {
                    duk_push_string(ctx->duk, val.c_str());
                }
                duk_put_global_string(ctx->duk, key.c_str());
                break;
            case '$':
                // dynamic (once)
                duk_eval_string(ctx->duk, val.substr(2, val.size()-3).c_str());
                duk_put_global_string(ctx->duk, key.c_str());
                break;
            case '#':
                // dynamic (every frame)
                dynamic_attr[key] = val.substr(2, val.size()-3);
                break;
        }
    }
}

void variables::init(scripting::context &ctx,
    std::map<std::string, std::string> const& attr)
{
    global = ctx.make_global();
    add_attr(attr);
}

std::string variables::dump() {
    auto ctx = global.use();
    auto duk = ctx->duk;
    duk_get_global_string(duk, "JSON");
    duk_get_prop_string(duk, -1, "stringify");
    duk_remove(duk, -2);
    duk_push_bare_object(duk);
    for (auto const& key : attr_keys) {
        duk_get_global_string(duk, key.c_str());
        duk_put_prop_string(duk, -2, key.c_str());
    }
    duk_call(duk, 1);
    std::string dump = duk_to_string(duk, -1);
    duk_pop(duk);
    return dump;
}

void variables::tick() {
    auto ctx = global.use();
    for (auto const& pair : dynamic_attr) {
        auto &key = pair.first;
        auto &js = pair.second;
        duk_eval_string(ctx->duk, js.c_str());
        duk_put_global_string(ctx->duk, key.c_str());
    }
}

void variables::finalize() {
    {
        auto ctx = global.use();
        duk_push_global_object(ctx->duk);
        for (auto const& key : attr_keys) {
            duk_del_prop_string(ctx->duk, -1, key.c_str());
        }
        duk_pop(ctx->duk);
        dynamic_attr.clear();
        attr_keys.clear();
    }
    global = {};
}

double variables::get_num(std::string const& key, double fallback) {
    auto ctx = global.use();
    duk_get_global_string(ctx->duk, key.c_str());
    double ret;
    if (duk_get_type(ctx->duk, -1) == DUK_TYPE_NUMBER) {
        ret = duk_get_number(ctx->duk, -1);
    }
    else {
        ret = fallback;
    }
    duk_pop(ctx->duk);
    return ret;
}

bool variables::get_bool(std::string const& key, bool fallback) {
    auto ctx = global.use();
    duk_get_global_string(ctx->duk, key.c_str());
    bool ret;
    if (duk_get_type(ctx->duk, -1) == DUK_TYPE_BOOLEAN) {
        ret = duk_get_boolean(ctx->duk, -1);
    }
    else {
        ret = fallback;
    }
    duk_pop(ctx->duk);
    return ret;
}

std::string variables::get_string(std::string const& key, std::string const& fallback) {
    auto ctx = global.use();
    duk_get_global_string(ctx->duk, key.c_str());
    std::string ret;
    if (duk_get_type(ctx->duk, -1) == DUK_TYPE_STRING) {
        ret = duk_get_string(ctx->duk, -1);
    }
    else {
        ret = fallback;
    }
    duk_pop(ctx->duk);
    return ret;
}

bool variables::get_bool(scripting::condition &cond) {
    auto ctx = global.use();
    return cond.eval(ctx);
}

bool variables::has(std::string const& key) {
    return attr_keys.count(key) > 0;
}

}
}
