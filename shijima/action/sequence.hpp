#pragma once
#include "base.hpp"
#include <vector>
#include <memory>

namespace shijima {
namespace action {

class sequence : public base {
private:
    //FIXME: Ideally this shouldn't be necessary
    std::shared_ptr<scripting::context> script_ctx;
protected:
    int action_idx = -1;
    std::shared_ptr<base> action;
    virtual std::shared_ptr<base> next_action();
public:
    std::vector<std::shared_ptr<base>> actions;
    virtual bool requests_interpolation() override;
    virtual void init(mascot::tick &ctx) override;
    virtual bool subtick(int idx) override;
    virtual void finalize() override;
};

}
}