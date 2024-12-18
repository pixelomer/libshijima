#pragma once
#include <shijima/scripting/variables.hpp>
#include <shijima/mascot/tick.hpp>
#include <shijima/broadcast/client.hpp>
#include <shijima/broadcast/server.hpp>
#include <shijima/log.hpp>

namespace shijima {
namespace action {

class base {
private:
    bool active = false;
    bool prevents_dragging;
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
};

}
}