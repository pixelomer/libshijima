#pragma once
#include "duktape/duktape.h"
#include <map>
#include <shijima/math.hpp>
#include <shijima/mascot/state.hpp>
#include <shijima/log.hpp>
#include <functional>
#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <stack>
#include <set>

namespace shijima {
namespace scripting {

class context {
private:
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
    duk_idx_t build_proxy();
    duk_idx_t push_function(std::function<duk_ret_t(duk_context *)> func,
        duk_idx_t nargs);
    void register_number_property(const char *name,
        std::function<double()> getter, std::function<void(double)> setter);
    void register_boolean_property(const char *name,
        std::function<bool()> getter, std::function<void(bool)> setter);

    duk_uarridx_t global_counter = 0;
    duk_uarridx_t globals_available = 0;
    std::vector<duk_uarridx_t> global_stack;
    bool has_global(duk_uarridx_t idx) {
        duk_push_global_object(duk);
        duk_get_prop_string(duk, -1, DUK_HIDDEN_SYMBOL("_globals"));
        duk_get_prop_index(duk, -1, idx);
        auto type = duk_get_type(duk, -1);
        duk_pop_3(duk);
        return type != DUK_TYPE_UNDEFINED;
    }
    bool is_global_active(duk_uarridx_t idx) {
        return std::find(global_stack.begin(), global_stack.end(),
            idx) != global_stack.end();
    }
    void create_global(duk_uarridx_t idx) {
        if (has_global(idx)) {
            throw std::logic_error("global index already in use");
        }
        duk_push_global_object(duk);
        duk_get_prop_string(duk, -1, DUK_HIDDEN_SYMBOL("_globals"));
        duk_push_bare_object(duk);
        duk_put_prop_index(duk, -2, idx);
        duk_pop_2(duk);
        globals_available++;
    }
    void delete_global(duk_uarridx_t idx) {
        if (!has_global(idx)) {
            throw std::logic_error("cannot delete non-existing global");
        }
        if (is_global_active(idx)) {
            throw std::logic_error("cannot delete active global");
        }
        duk_push_global_object(duk);
        duk_get_prop_string(duk, -1, DUK_HIDDEN_SYMBOL("_globals"));
        duk_del_prop_index(duk, -1, idx);
        duk_pop_2(duk);
        globals_available--;
    }
    void push_global(duk_uarridx_t idx) {
        if (!has_global(idx)) {
            throw std::logic_error("no such global");
        }
        global_stack.push_back(idx);
    }
    void pop_global(duk_uarridx_t idx) {
        if (global_stack.empty() || global_stack[global_stack.size()-1] != idx) {
            throw std::logic_error("idx != global_stack.top()");
        }
        global_stack.erase(global_stack.begin() + global_stack.size() - 1);
    }
    duk_uarridx_t next_global_idx() {
        while (has_global(++global_counter));
        auto idx = global_counter;
        return idx;
    }

    std::shared_ptr<bool> invalidated_flag;
public:
    // Holds a bare pointer to the context. The context should therefore
    // outlive any global. In case it doesn't, the invalidated flag
    // will prevent bad memory access.
    class global {
    private:
        friend class context;
        duk_uarridx_t idx;
        context *ctx;
        bool is_active;
        std::shared_ptr<bool> invalidated;
        void check_valid() {
            if (invalidated != nullptr && *invalidated) {
                ctx = nullptr;
                if (is_active) {
                    throw std::logic_error("global was invalidated while active");
                }
                is_active = false;
                invalidated = nullptr;
            }
        }
        void init() {
            check_valid();
            if (is_active) {
                throw std::logic_error("init() called on active global");
            }
            is_active = true;
            ctx->push_global(idx);
        }
        void finalize() {
            check_valid();
            if (!is_active) {
                throw std::logic_error("finalize() called on inactive global");
            }
            is_active = false;
            ctx->pop_global(idx);
        }
        global(context *ctx, unsigned long idx,
            std::shared_ptr<bool> invalidated): idx(idx), ctx(ctx),
            is_active(false), invalidated(invalidated) {}
    public:
        // Holds a bare pointer to the global. Intended to be used on the stack.
        // Must outlive the global. Will cause a crash if the global is freed
        // first.
        class active {
        private:
            friend class global;
            global *owner;
            active(global *owner): owner(owner) {}
        public:
            context *get() {
                return owner->ctx;
            }
            context *operator->() {
                return owner->ctx;
            }
            active(active const&) = delete;
            active& operator=(active const&) = delete;
            active(active &&rhs) {
                if (owner != nullptr) {
                    owner->finalize();
                }
                owner = rhs.owner;
                rhs.owner = nullptr;
            }
            active &operator=(active &&rhs) {
                if (owner != nullptr) {
                    owner->finalize();
                }
                owner = rhs.owner;
                rhs.owner = nullptr;
                return *this;
            }
            ~active() {
                if (owner != nullptr) {
                    owner->finalize();
                }
            }
        };
        global(global const&) = delete;
        global& operator=(global const&) = delete;
        #define move() do { \
            check_valid(); \
            if (is_active || rhs.is_active) { \
                throw std::logic_error("cannot destroy active global"); \
            } \
            if (ctx != nullptr) { \
                ctx->delete_global(idx); \
            } \
            invalidated = rhs.invalidated; \
            ctx = rhs.ctx; \
            idx = rhs.idx; \
            rhs.ctx = nullptr; \
            rhs.invalidated = nullptr; \
        } while(0)
        global(global &&rhs): ctx(nullptr), is_active(false) {
            move();
        }
        global &operator=(global &&rhs) {
            move();
            return *this;
        }
        #undef move
        ~global() {
            check_valid();
            if (ctx != nullptr) {
                if (is_active) {
                    finalize();
                }
                ctx->delete_global(idx);
            }
        }
        active use() {
            init();
            return { this };
        }
        bool valid() {
            return ctx != nullptr;
        }
        global(): ctx(nullptr), is_active(false) {}
    };
    duk_context *duk;
    std::shared_ptr<mascot::state> state;
    global make_global() {
        auto idx = next_global_idx();
        create_global(idx);
        return { this, idx, invalidated_flag };
    }
    bool eval_bool(std::string const& js) {
        duk_eval_string(duk, js.c_str());
        bool ret = duk_to_boolean(duk, -1);
        if (get_log_level() & SHIJIMA_LOG_JAVASCRIPT) {
            std::string m_js = js;
            size_t i;
            for (i=0; (i = m_js.find_first_of("\r\t\n", i)) != std::string::npos;) {
                m_js[i] = ' ';
            }
            log("\"" + m_js + "\" = " + (ret ? "true" : "false"));
        }
        duk_pop(duk);
        return ret;
    }
    double eval_number(std::string js) {
        duk_eval_string(duk, js.c_str());
        double ret = duk_to_number(duk, -1);
        if (get_log_level() & SHIJIMA_LOG_JAVASCRIPT) {
            std::string m_js = js;
            size_t i;
            for (i=0; (i = m_js.find_first_of("\r\t\n", i)) != std::string::npos;) {
                m_js[i] = ' ';
            }
            log("\"" + m_js + "\" = " + std::to_string(ret));
        }
        duk_pop(duk);
        return ret;
    }
    std::string eval_string(std::string js) {
        duk_eval_string(duk, js.c_str());
        std::string ret = duk_to_string(duk, -1);
        if (get_log_level() & SHIJIMA_LOG_JAVASCRIPT) {
            std::string m_js = js;
            size_t i;
            for (i=0; (i = m_js.find_first_of("\r\t\n", i)) != std::string::npos;) {
                m_js[i] = ' ';
            }
            log("\"" + m_js + "\" = \"" + ret + "\"");
        }
        duk_pop(duk);
        return ret;
    }
    std::string eval_json(std::string js) {
        duk_get_global_string(duk, "JSON");
        duk_get_prop_string(duk, -1, "stringify");
        duk_remove(duk, -2);
        duk_eval_string(duk, js.c_str());
        duk_call(duk, 1);
        std::string ret = duk_to_string(duk, -1);
        duk_pop(duk);
        return ret;
    }
    void eval(std::string js) {
        duk_eval_string_noresult(duk, js.c_str());
    }
    context() {
        invalidated_flag = std::make_shared<bool>(false);
        duk = duk_create_heap_default();
        build_mascot();
        duk_put_global_string(duk, "mascot");
        build_console();
        duk_put_global_string(duk, "console");
        build_proxy();
        duk_set_global_object(duk);

        // initial global to prevent the real global
        // from being modified
        auto idx = next_global_idx();
        create_global(idx);
        push_global(idx);
    }
    ~context() {
        *invalidated_flag = true;
        duk_destroy_heap(duk);
    }
    context &operator=(context const&) = delete;
    context &operator=(context&&) = delete;
    context(context const&) = delete;
    context(context&&) = delete;
};

}
}