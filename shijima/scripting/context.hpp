#pragma once
#include "duktape/duktape.h"
#include <map>
#include <shijima/math.hpp>
#include <shijima/mascot/state.hpp>
#include <functional>
#include <iostream>
#include <memory>

namespace shijima {
namespace scripting {

class context {
private:
    static int contexts_in_use;
    static std::vector<context *> available_contexts;
    static math::vec2 duk_to_point(duk_context *ctx, duk_idx_t idx) {
        math::vec2 point;
        duk_get_prop_string(ctx, idx, "x");
        point.x = duk_to_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_string(ctx, idx, "y");
        point.y = duk_to_number(ctx, -1);
        duk_pop(ctx);
        return point;
    }

    static duk_ret_t duk_getter(duk_context *ctx);
    static duk_ret_t duk_setter(duk_context *ctx);

    duk_idx_t build_mascot();
    duk_idx_t build_console();
    duk_idx_t build_area(std::function<mascot::environment::area&()> getter);
    void put_prop(duk_idx_t obj, std::string const& name);
    void put_prop_functions(std::string const& prop_name);

    template<typename T>
    duk_idx_t build_border(std::function<T()> getter) {
        auto border = duk_push_bare_object(duk);

        // leftBorder.isOn(point)
        //   where point = { x: number, y: number }
        push_function([getter](duk_context *ctx) -> duk_ret_t {
            auto point = duk_to_point(ctx, 0);
            duk_push_boolean(ctx, getter().is_on(point));
            return 1;
        }, 1);
        duk_put_prop_string(duk, -2, "isOn");

        return border;
    }
    
    duk_idx_t build_environment();
    duk_idx_t build_rectangle(std::function<math::rec&()> getter);
    duk_idx_t build_vec2(std::function<math::vec2&()> getter);
    duk_idx_t build_dvec2(std::function<mascot::environment::dvec2&()> getter);
    static void duk_write_vararg_log(duk_context *ctx, std::ostream &out);
    static duk_ret_t duk_console_log(duk_context *ctx);
    static duk_ret_t duk_console_error(duk_context *ctx);
    static duk_ret_t duk_function_callback(duk_context *ctx);
    static duk_ret_t duk_finalizer_callback(duk_context *ctx);
    duk_idx_t push_function(std::function<duk_ret_t(duk_context *)> func,
        duk_idx_t nargs);
    void register_number_property(const char *name,
        std::function<double()> getter, std::function<void(double)> setter);
    void register_boolean_property(const char *name,
        std::function<bool()> getter, std::function<void(bool)> setter);
public:
    duk_context *duk;
    static std::shared_ptr<context> get() {
        if (available_contexts.size() == 0) {
            context *ctx = new context();
            //std::cerr << "new context: " << (void *)ctx << std::endl;
            available_contexts.push_back(ctx);
        }
        contexts_in_use++;
        if (contexts_in_use >= 10) {
            std::cerr << "too many contexts in use: " << contexts_in_use
                << std::endl;
        }
        auto ctx = available_contexts.back();
        //std::cerr << "will lend context: " << (void *)ctx << std::endl;
        available_contexts.pop_back();
        return std::shared_ptr<context>(ctx, [](context *ctx) {
            //std::cerr << "will recycle context: " << (void *)ctx << std::endl;
            context::contexts_in_use--;
            context::available_contexts.push_back(ctx);
        });
    }
    std::shared_ptr<mascot::state> state;
    bool eval_bool(std::string js) {
        duk_eval_string(duk, js.c_str());
        bool ret = duk_to_boolean(duk, -1);
        duk_pop(duk);
        return ret;
    }
    double eval_number(std::string js) {
        duk_eval_string(duk, js.c_str());
        double ret = duk_to_number(duk, -1);
        duk_pop(duk);
        return ret;
    }
    std::string eval_string(std::string js) {
        duk_eval_string(duk, js.c_str());
        std::string ret = duk_to_string(duk, -1);
        duk_pop(duk);
        return ret;
    }
    void eval(std::string js) {
        duk_eval_string_noresult(duk, js.c_str());
    }
    context() {
        duk = duk_create_heap_default();
        build_mascot();
        duk_put_global_string(duk, "mascot");
        build_console();
        duk_put_global_string(duk, "console");
    }
    ~context() {
        duk_destroy_heap(duk);
    }
};

}
}