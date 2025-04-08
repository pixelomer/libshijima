#pragma once

// 
// libshijima - C++ library for shimeji desktop mascots
// Copyright (C) 2024-2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#include "../config.hpp"
#include <shijima/scripting/variables.hpp>
#include <shijima/mascot/tick.hpp>
#include <shijima/broadcast/client.hpp>
#include <shijima/broadcast/server.hpp>
#include <shijima/log.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>

namespace shijima {
namespace action {

class base {
private:
    bool active = false;
    math::vec2 target_offset;
protected:
    std::shared_ptr<mascot::state> mascot;
    scripting::variables vars;
    broadcast::server server;
    broadcast::client client;
    int start_time;

    void reset_elapsed() {
        start_time = mascot->time;
    }
    int elapsed() {
        return mascot->time - start_time;
    }
    template<typename T>
    T dx(T dx) {
        return (mascot->looking_right ? -1 : 1) * dx;
    }
    template<typename T>
    T dy(T dy) {
        return dy;
    }
public:
    virtual bool requests_vars();
    virtual bool requests_broadcast();
    virtual bool requests_interpolation();

    std::map<std::string, std::string> init_attr;

    // Initializes the action, resetting any internal state as necessary.
    // If init() throws an exception, the object should be inactive afterwards.
    virtual void init(mascot::tick &ctx);

    // Returns false if execution should immediately advance to the
    // next action. The action should return true for the last frame
    // and return false for the frame after the last.
    virtual bool tick();
    virtual void finalize();
    virtual ~base() {}

    // Actions may implement subtick(int idx) instead to support subticks
    // and produce smoother animation. The default implementation creates a
    // linear motion based on the position change from tick(). 
    virtual bool subtick(int idx);

    template<class Archive>
    void serialize(Archive &ar) {
        ar(init_attr);
    }
};

}
}