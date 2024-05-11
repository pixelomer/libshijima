#pragma once
#include <vector>
#include <memory>
#include "base.hpp"
#include <shijima/scripting/context.hpp>
#include <shijima/mascot/state.hpp>
#include <random>

namespace shijima {
namespace behavior {

class manager {
private:
    static int random(int min, int max) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
        return (int)dist(rng);
    }

    list initial_list;
    list next_list;
    scripting::context::global global;
public:
    manager(): initial_list(), next_list() {
    }
    manager(scripting::context &ctx, list initial_list,
        std::string const& first_behavior): initial_list(initial_list), next_list()
    {
        global = ctx.make_global();
        set_next(first_behavior);
    }
    void set_next(std::string const& next_name) {
        next_list = {};
        next_list.children.push_back(initial_list.find(next_name));
    }
    std::shared_ptr<base> next(std::shared_ptr<mascot::state> state) {
        auto ctx = global.use();
        ctx->state = state;
        auto flat = next_list.flatten(*ctx.get());

        std::shared_ptr<base> behavior;
        
        if (flat.size() == 1) {
            // Use the only option
            behavior = flat[0];
        }
        else {
            // Sum all frequencies
            int freq_sum = 0;
            for (auto &option : flat) {
                freq_sum += option->frequency;
            }
            if (freq_sum == 0) {
                return nullptr;
            }

            // Pick a random behavior
            int counter = 0;
            int dice = random(0, freq_sum-1);
            for (auto &option : flat) {
                counter += option->frequency;
                if (counter > dice) {
                    behavior = option;
                    break;
                }
            }
        }

        // Behavior may be a reference, use the original
        if (behavior->referenced != nullptr) {
            behavior = behavior->referenced;
        }

        // Adjust next_list
        if (!behavior->add_next) {
            // If "Add"="false", only use the options provided by the behavior
            next_list = *behavior->next_list;
        }
        else {
            // Otherwise, merge the initial list and the behavior list
            next_list = {};
            next_list.sublists.push_back(initial_list);
            if (behavior->next_list != nullptr) {
                next_list.sublists.push_back(*behavior->next_list);
            }
        }

        return behavior;
    }
};

}
}