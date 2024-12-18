#include "base.hpp"
#include "list.hpp"

namespace shijima {
namespace behavior {

base::base(std::string const& name, int freq, bool hidden,
    scripting::condition const& cond): name(name), frequency(freq),
    hidden(hidden), condition(cond) {}

}
}