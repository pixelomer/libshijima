#pragma once
#include <map>
#include <set>
#include "condition.hpp"
#include "context.hpp"

namespace shijima {
namespace scripting {

class variables {
private:
    std::map<std::string, std::string> dynamic_attr;
    std::set<std::string> attr_keys;
    scripting::context::global global;
    static signed char dynamic_prefix(std::string const& str);
    static bool is_num(std::string const& str);
public:
    variables();
    void add_attr(std::map<std::string, double> const& attr);
    void add_attr(std::map<std::string, std::string> const& attr);
    void init(scripting::context &ctx,
        std::map<std::string, std::string> const& attr);
    std::string dump();
    void tick();
    void finalize();
    double get_num(std::string const& key, double fallback = 0.0);
    bool get_bool(std::string const& key, bool fallback = false);
    std::string get_string(std::string const& key, std::string const& fallback = "");
    bool get_bool(scripting::condition &cond);
    bool has(std::string const& key);
};

}
}