#pragma once
#include <memory>
#include <map>
#include <set>
#include <cstdlib>
#include "context.hpp"

namespace shijima {
namespace scripting {

class variables {
private:
    std::map<std::string, std::string> dynamic_attr;
    std::set<std::string> attr_keys;
    std::shared_ptr<context> ctx;
    static char dynamic_prefix(std::string const& str) {
        if (str.size() >= 3 && str[1] == '{' &&
            str[str.size()-1] == '}' &&
            (str[0] == '$' || str[0] == '#'))
        {
            return str[0];
        }
        return -1;
    }
    static bool is_num(std::string const& str) {
        char *end = NULL;
        double val = strtod(str.c_str(), &end);
        return end != str.c_str() && *end == '\0' && val != HUGE_VAL;
    }
public:
    void add_attr(std::map<std::string, std::string> const& attr) {
        for (auto const& pair : attr) {
            auto &key = pair.first;
            auto val = pair.second;
            if (val == "null" || val == "true" || val == "false") {
                val = "${" + val + "}";
            }
            dynamic_attr.erase(key);
            attr_keys.insert(key);
            char c = dynamic_prefix(val);
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
    void init(std::shared_ptr<mascot::state> mascot,
        std::map<std::string, std::string> const& attr)
    {
        ctx = context::get();
        ctx->state = mascot;
        add_attr(attr);
    }
    void tick() {
        for (auto const& pair : dynamic_attr) {
            auto &key = pair.first;
            auto &js = pair.second;
            duk_eval_string(ctx->duk, js.c_str());
            duk_put_global_string(ctx->duk, key.c_str());
        }
    }
    void finalize() {
        duk_push_global_object(ctx->duk);
        for (auto const& key : attr_keys) {
            duk_del_prop_string(ctx->duk, -1, key.c_str());
        }
        duk_pop(ctx->duk);
        dynamic_attr.clear();
        attr_keys.clear();
        this->ctx->state = nullptr;
        this->ctx = nullptr;
    }
    double get_num(std::string const& key, double fallback = 0.0) {
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
    bool get_bool(std::string const& key, bool fallback = false) {
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
    bool has(std::string const& key) {
        return attr_keys.count(key) > 0;
    }
};

}
}