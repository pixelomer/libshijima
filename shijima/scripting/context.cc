#include "context.hpp"

namespace shijima {
namespace scripting {

duk_ret_t context::duk_getter(duk_context *ctx) {
    // 0      1      2
    // [func] [this] [val]
    duk_push_current_function(ctx); // 0
    duk_push_this(ctx); // 1
    duk_get_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("prop")); // 2
    duk_get_prop(ctx, -2); // 2
    duk_copy(ctx, -1, -3); 
    duk_pop_2(ctx);
    return 1;
}

//FIXME: Setting objects like this will overwrite getters and setters
duk_ret_t context::duk_setter(duk_context *ctx) {
    // 0     1      2      3           4
    // [arg] [func] [this] [prop_name] [copy of arg]
    duk_push_current_function(ctx);
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("prop"));
    duk_push_undefined(ctx);
    duk_copy(ctx, 0, duk_get_top_index(ctx));
    duk_put_prop(ctx, -3);
    duk_pop_2(ctx);
    return 1;
}

void context::put_prop_functions(std::string const& prop_name) {
    if (prop_name.size() <= 0) {
        throw std::logic_error("name.size() <= 0");
    }

    auto suffix = prop_name;
    suffix[0] = std::toupper(suffix[0]);

    // .getProp()
    auto getter = "get" + suffix;
    duk_push_c_function(duk, duk_getter, 0);
    duk_push_string(duk, prop_name.c_str());
    duk_put_prop_string(duk, -2, DUK_HIDDEN_SYMBOL("prop"));
    duk_put_prop_string(duk, -2, getter.c_str());

    // .setProp(value)
    auto setter = "set" + suffix;
    duk_push_c_function(duk, duk_setter, 1);
    duk_push_string(duk, prop_name.c_str());
    duk_put_prop_string(duk, -2, DUK_HIDDEN_SYMBOL("prop"));
    duk_put_prop_string(duk, -2, setter.c_str());
}

void context::put_prop(duk_idx_t obj, std::string const& prop_name) {
    if (prop_name.size() <= 0) {
        throw std::logic_error("name.size() <= 0");
    }
    duk_put_prop_string(duk, obj, prop_name.c_str());
    put_prop_functions(prop_name);
}

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
    duk_get_prop_string(duk, -1, DUK_HIDDEN_SYMBOL("stdfunc"));
    void *target_pt = duk_get_pointer(duk, -1);
    auto target = static_cast<std::function<duk_ret_t(duk_context *)>
        *>(target_pt);
    duk_pop(duk);
    return (*target)(duk);
}

duk_ret_t context::duk_finalizer_callback(duk_context *duk) {
    duk_push_current_function(duk);
    duk_get_prop_string(duk, -1, DUK_HIDDEN_SYMBOL("stdfunc"));
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
    duk_put_prop_string(duk, -2, DUK_HIDDEN_SYMBOL("stdfunc"));
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
            setter(duk_get_number(duk, 0));
            return (duk_ret_t)0;
        }, 1);
        idx -= 1;
        flags |= DUK_DEFPROP_HAVE_SETTER;
    }
    if (idx == -2) {
        throw std::invalid_argument("Both getter and setter are null.");
    }
    duk_def_prop(duk, idx, flags);
    put_prop_functions(name);
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
            setter(duk_get_boolean(duk, 0));
            return (duk_ret_t)0;
        }, 1);
        idx -= 1;
        flags |= DUK_DEFPROP_HAVE_SETTER;
    }
    if (idx == -2) {
        throw std::invalid_argument("Both getter and setter are null.");
    }
    duk_def_prop(duk, idx, flags);
    put_prop_functions(name);
}

duk_idx_t context::build_console() {
    auto console = duk_push_bare_object(duk);
    duk_push_c_function(duk, duk_console_log, DUK_VARARGS);
    put_prop(-2, "log");
    duk_push_c_function(duk, duk_console_error, DUK_VARARGS);
    put_prop(-2, "error");
    return console;
}

duk_idx_t context::build_mascot() {
    auto mascot = duk_push_bare_object(duk);
    
    // mascot.bounds
    build_rectangle([this]() -> math::rec& { return this->state->bounds; });
    put_prop(-2, "bounds");

    // mascot.anchor
    build_vec2([this]() -> math::vec2& { return this->state->anchor; });
    put_prop(-2, "anchor");

    // mascot.lookRight
    register_boolean_property("lookRight",
        [this]() { return this->state->looking_right; },
        [this](bool value) { this->state->looking_right = value; });
    
    // mascot.environment
    build_environment();
    put_prop(-2, "environment");
    
    return mascot;
}
    
duk_idx_t context::build_area(std::function<mascot::environment::area&()> getter) {
    auto area = duk_push_bare_object(duk);

    // area.rightBorder
    build_border<mascot::environment::vborder>([getter]() { return getter().right_border(); });
    put_prop(-2, "rightBorder");

    // area.leftBorder
    build_border<mascot::environment::vborder>([getter]() { return getter().left_border(); });
    put_prop(-2, "leftBorder");

    // area.topBorder
    build_border<mascot::environment::hborder>([getter]() { return getter().top_border(); });
    put_prop(-2, "topBorder");

    // area.bottomBorder
    build_border<mascot::environment::hborder>([getter]() { return getter().bottom_border(); });
    put_prop(-2, "bottomBorder");

    // area.width
    register_number_property("width", [getter]() { return getter().width(); }, \
        nullptr);

    // area.height
    register_number_property("height", [getter]() { return getter().height(); }, \
        nullptr);
    
    // area.visible
    register_boolean_property("visible", [getter]() { return getter().visible(); },
        nullptr);

    // area.left, area.right, area.top, area.bottom
    #define reg(side) \
        register_number_property(#side, [getter]() { return getter().side; }, \
        [getter](double val) { getter().side = val; })
    reg(left); reg(right); reg(top); reg(bottom);
    #undef reg
    
    return area;
}

duk_idx_t context::build_proxy() {
    duk_push_global_object(duk);

    // proxy._activeGlobal
    duk_push_literal(duk, "_activeGlobal");
    push_function([this](duk_context *duk){
        if (global_stack.size() == 0) {
            duk_push_this(duk);
            return 1;
        }
        auto active = global_stack[global_stack.size()-1];
        duk_push_this(duk); // -3
        duk_get_prop_literal(duk, -1, DUK_HIDDEN_SYMBOL("_globals")); // -2
        duk_get_prop_index(duk, -1, active); // -1
        duk_copy(duk, -1, duk_normalize_index(duk, -3));
        duk_pop_2(duk);
        return 1;
    }, 0);
    duk_def_prop(duk, -3, DUK_DEFPROP_HAVE_GETTER);

    // proxy._globalCount
    duk_push_literal(duk, "_globalCount");
    push_function([this](duk_context *duk){
        duk_push_number(duk, this->globals_available);
        return 1;
    }, 0);
    duk_def_prop(duk, -3, DUK_DEFPROP_HAVE_GETTER);

    // proxy[HiddenSymbol("_globals")]
    duk_push_literal(duk, DUK_HIDDEN_SYMBOL("_globals"));
    duk_push_bare_object(duk);
    duk_def_prop(duk, -3, DUK_DEFPROP_HAVE_VALUE);
    duk_pop(duk);
    const char *builder =
        "(function(target){"
        "    return new Proxy(target, {"
        "        defineProperty(target, property, descriptor) {"
        "            if (property === \"_activeGlobal\") return;"
        "            const real = target._activeGlobal;"
        "            return Reflect.defineProperty(real, property, descriptor);"
        "        },"
        "        \"set\": function(target, property, value, receiver) {"
        "            if (property === \"_activeGlobal\") return;"
        "            const real = target._activeGlobal;"
        "            return Reflect.set(real, property, value/*, receiver*/);"
        "        },"
        "        deleteProperty(target, property, descriptor) {"
        "            if (property === \"_activeGlobal\") return;"
        "            const real = target._activeGlobal;"
        "            return Reflect.deleteProperty(real, property, descriptor);"
        "        },"
        "        \"get\": function(target, property, receiver) {"
        "            const real = target._activeGlobal;"
        "            if (property in real) {"
        "                return Reflect.get(real, property/*, receiver*/);"
        "            }"
        "            else {"
        "                return Reflect.get(target, property/*, receiver*/);"
        "            }"
        "        },"
        "        has(target, property) {"
        "            const real = target._activeGlobal;"
        "            return (property in real) || (property in target);"
        "        }"
        "    })"
        "})(globalThis)";
    duk_eval_string(duk, builder);
    return duk_get_top_index(duk);
}
    
duk_idx_t context::build_environment() {
    auto env = duk_push_bare_object(duk);

    // environment.floor
    build_border<mascot::environment::hborder>([this]() { return this->state->env.floor; });
    put_prop(-2, "floor");

    // environment.ceiling
    build_border<mascot::environment::hborder>([this]() { return this->state->env.ceiling; });
    put_prop(-2, "ceiling");

    // environment.workArea
    build_area([this]() -> mascot::environment::area& { return this->state->env.work_area; });
    put_prop(-2, "workArea");

    // environment.screen
    build_area([this]() -> mascot::environment::area& { return this->state->env.screen; });
    put_prop(-2, "screen");

    // environment.activeIE
    build_area([this]() -> mascot::environment::area& { return this->state->env.active_ie; });
    put_prop(-2, "activeIE");

    // environment.cursor
    build_dvec2([this]() -> mascot::environment::dvec2& { return this->state->env.cursor; });
    put_prop(-2, "cursor");

    return env;
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

duk_idx_t context::build_dvec2(std::function<mascot::environment::dvec2&()> getter) {
    auto vec2 = duk_push_bare_object(duk);
    #define reg(x) \
        register_number_property(#x, [getter]() { return getter().x; }, \
        [getter](double val) { getter().x = val; })
    reg(x); reg(y); reg(dx); reg(dy);
    #undef reg
    return vec2;
}

}
}