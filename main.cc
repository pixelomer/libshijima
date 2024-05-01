#include <shijima/mascot/mascot.hpp>
#include <shijima/parser.hpp>
#include <sstream>
#include <fstream>

int main() {
    std::string actions_xml;
    {
        std::stringstream buf;
        std::ifstream f("tests/shimeji/test1/actions.xml");
        buf << f.rdbuf();
        f.close();
        actions_xml = buf.str();
    }

    shijima::parser parser;
    parser.parse(actions_xml, "");

    shijima::mascot::mascot mascot;
    mascot.state->anchor.x = 500;
    mascot.state->anchor.y = 500;
    mascot.state->env.work_area.bottom = 1080;
    mascot.state->env.work_area.top = 0;
    mascot.state->env.work_area.left = 0;
    mascot.state->env.work_area.right = 1920;
    mascot.state->env.cursor = { 50, 50 };
    auto action = parser.actions.at("Falling");
    action->init(mascot.state, {});
    while (action->tick()) {
        std::cout << "tick" << std::endl;
        mascot.state->time++;
        (void)0;
    }
    action->finalize();
    mascot.state->anchor.y = 500;
    action->init(mascot.state, {});
    while (action->tick()) {
        std::cout << "tick" << std::endl;
        mascot.state->time++;
        (void)0;
    }
    action->finalize();
}