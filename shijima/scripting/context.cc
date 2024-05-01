#include "context.hpp"

namespace shijima {
namespace scripting {

int context::contexts_in_use = 0;
std::vector<context *> context::available_contexts;

void context::duk_write_vararg_log(duk_context *duk, std::ostream &out) {
    duk_idx_t top = duk_get_top(duk);
    for (duk_idx_t i=0; i<top; i++) {
        out << duk_to_string(duk, i);
        if ((i + 1) != top) out << " ";
    }
    out << std::endl;
}

duk_ret_t context::duk_console_log(duk_context *duk) {
    duk_write_vararg_log(duk, std::cout);
    return 0;
}

duk_ret_t context::duk_console_error(duk_context *duk) {
    duk_write_vararg_log(duk, std::cerr);
    return 0;
}

duk_ret_t context::duk_function_callback(duk_context *duk) {
    duk_push_current_function(duk);
    duk_get_prop_string(duk, -1, "\xFF" "stdfunc");
    void *target_pt = duk_get_pointer(duk, -1);
    auto target = static_cast<std::function<duk_ret_t(duk_context *)>
        *>(target_pt);
    duk_pop(duk);
    return (*target)(duk);
}

duk_ret_t context::duk_finalizer_callback(duk_context *duk) {
    duk_push_current_function(duk);
    duk_get_prop_string(duk, -1, "\xFF" "stdfunc");
    void *target_pt = duk_get_pointer(duk, -1);
    auto target = static_cast<std::function<duk_ret_t(duk_context *,
        void *)> *>(target_pt);
    delete target;
    duk_pop(duk);
    return 0;
}

//FIXME: potential arbitrary code execution by malicious shimeji
duk_idx_t context::push_function(std::function<duk_ret_t(duk_context *)> func,
    duk_idx_t nargs)
{
    auto heap_func = new std::function<duk_ret_t(duk_context *)>(func);
    duk_idx_t idx = duk_push_c_function(duk, duk_function_callback, nargs);
    duk_push_pointer(duk, static_cast<void *>(heap_func));
    duk_put_prop_string(duk, -2, "\xFF" "stdfunc");
    duk_push_c_function(duk, duk_finalizer_callback, 1);
    duk_set_finalizer(duk, -2);
    return idx;
}

void context::register_number_property(const char *name,
    std::function<double()> getter, std::function<void(double)> setter)
{
    duk_push_string(duk, name);
    duk_uint_t flags = DUK_DEFPROP_SET_ENUMERABLE;
    duk_idx_t idx = -2;
    if (getter != nullptr) {
        push_function([getter](duk_context *duk){
            duk_push_number(duk, getter());
            return (duk_ret_t)1;
        }, 0);
        idx -= 1;
        flags |= DUK_DEFPROP_HAVE_GETTER;
    }
    if (setter != nullptr) {
        push_function([setter](duk_context *duk){
            setter(duk_get_number(duk, -1));
            return (duk_ret_t)0;
        }, 1);
        idx -= 1;
        flags |= DUK_DEFPROP_HAVE_SETTER;
    }
    if (idx == -2) {
        throw std::invalid_argument("Both getter and setter are null.");
    }
    duk_def_prop(duk, idx, flags);
}

void context::register_boolean_property(const char *name,
    std::function<bool()> getter, std::function<void(bool)> setter)
{
    duk_push_string(duk, name);
    duk_uint_t flags = DUK_DEFPROP_SET_ENUMERABLE;
    duk_idx_t idx = -2;
    if (getter != nullptr) {
        push_function([getter](duk_context *duk){
            duk_push_boolean(duk, getter());
            return (duk_ret_t)1;
        }, 0);
        idx -= 1;
        flags |= DUK_DEFPROP_HAVE_GETTER;
    }
    if (setter != nullptr) {
        push_function([setter](duk_context *duk){
            setter(duk_get_boolean(duk, -1));
            return (duk_ret_t)0;
        }, 1);
        idx -= 1;
        flags |= DUK_DEFPROP_HAVE_SETTER;
    }
    if (idx == -2) {
        throw std::invalid_argument("Both getter and setter are null.");
    }
    duk_def_prop(duk, idx, flags);
}

duk_idx_t context::build_console() {
    auto console = duk_push_bare_object(duk);
    duk_push_c_function(duk, duk_console_log, DUK_VARARGS);
    duk_put_prop_string(duk, -2, "log");
    duk_push_c_function(duk, duk_console_error, DUK_VARARGS);
    duk_put_prop_string(duk, -2, "error");
    return console;
}

duk_idx_t context::build_mascot() {
    auto mascot = duk_push_bare_object(duk);
    
    // mascot.bounds
    build_rectangle([this]() -> math::rec& { return this->state->bounds; });
    duk_put_prop_string(duk, -2, "bounds");

    // mascot.anchor
    build_vec2([this]() -> math::vec2& { return this->state->anchor; });
    duk_put_prop_string(duk, -2, "anchor");

    // mascot.lookRight
    register_boolean_property("looksRight",
        [this]() { return this->state->looking_right; },
        [this](bool value) { this->state->looking_right = value; });
    
    return mascot;
}

duk_idx_t context::build_rectangle(std::function<math::rec&()> getter) {
    auto rect = duk_push_bare_object(duk);
    #define reg(x) \
        register_number_property(#x, [getter]() { return getter().x; }, \
        [getter](double val) { getter().x = val; })
    reg(x); reg(y); reg(width); reg(height);
    #undef reg
    return rect;
}

duk_idx_t context::build_vec2(std::function<math::vec2&()> getter) {
    auto vec2 = duk_push_bare_object(duk);
    #define reg(x) \
        register_number_property(#x, [getter]() { return getter().x; }, \
        [getter](double val) { getter().x = val; })
    reg(x); reg(y);
    #undef reg
    return vec2;
}

}
}