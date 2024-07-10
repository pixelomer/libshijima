#pragma once
#include "state.hpp"
#include <shijima/scripting/context.hpp>
#include <shijima/log.hpp>
#include <memory>

namespace shijima {
namespace mascot {

class tick {
public:
    std::shared_ptr<scripting::context> script;
    std::map<std::string, std::string> extra_attr;

    class init_limit_error : public std::logic_error {
    public:
        init_limit_error(): std::logic_error("init() called too many times") {}
    };
private:
    std::shared_ptr<int> init_count;
    tick(std::shared_ptr<scripting::context> script,
        std::map<std::string, std::string> const& extra_attr,
        std::shared_ptr<int> init_count): script(script),
        extra_attr(extra_attr), init_count(init_count) {}
public:
    void will_init() {
        if (++*init_count >= 20) {
            throw init_limit_error();
        }
    }
    void reset() {
        *init_count = 0;
    }
    tick(std::shared_ptr<scripting::context> script,
        std::map<std::string, std::string> const& extra_attr):
        tick(script, extra_attr, std::make_shared<int>()) {}
    tick(): tick(nullptr, {}, std::make_shared<int>()) {}
    
    mascot::tick overlay(std::map<std::string, std::string> const& new_attr) {
        std::map<std::string, std::string> extra_copy = extra_attr;
        for (std::pair<const std::string, std::string> const& pair : new_attr) {
            extra_copy[pair.first] = pair.second;
        }
        return { script, extra_copy, init_count };
    }
};

}
}