#include <gtest/gtest.h>
#include <shijima/shijima.hpp>
#include <shijima/scripting/context.hpp>
#include <fstream>
#include <sstream>

struct shimeji_test_data {
    std::string actions_xml;
    std::string behaviors_xml;
};

shimeji_test_data load_shimeji(std::string name) {
    shimeji_test_data data;
    std::stringstream buf;
    {
        std::ifstream f("tests/shimeji/" + name + "/actions.xml");
        buf << f.rdbuf();
        data.actions_xml = buf.str();
    }
    buf.clear();
    {
        std::ifstream f("tests/shimeji/" + name + "/behaviors.xml");
        buf << f.rdbuf();
        data.behaviors_xml = buf.str();
    }
    return data;
}

TEST(Parser, ParseWithoutException) {
    auto shimeji = load_shimeji("test1");
    shijima::parser parser;
    parser.parse(shimeji.actions_xml, shimeji.behaviors_xml);
}

TEST(Scripting, MascotState) {
    shijima::scripting::context ctx;
    ctx.state = std::make_shared<shijima::mascot::state>();
    ctx.state->looking_right = true;
    ASSERT_EQ(ctx.eval_bool("mascot.looksRight"), true);
    ASSERT_EQ(ctx.eval_bool("mascot.looksRight = !mascot.looksRight"), false);
    ASSERT_EQ(ctx.state->looking_right, false);
    ctx.state->bounds.height = 105;
    ASSERT_EQ(ctx.eval_number("mascot.bounds.height"), 105);

    ctx.eval("console.log(JSON.stringify(globalThis, null, 2))");
}