# libshijima

C++ library for shimeji desktop mascots

## Usage

libshijima itself does not implement a GUI. Rather, it provides an easy API for parsing configurations files and interacting with shimeji. For an example usage of libshijima, see [main.cc](blob/main/main.cc) which implements an SDL application for interacting with shimeji.

The main entry point for libshijima is `shijima::mascot::factory`. An app using libshijima may look something like the following:

```cpp
using namespace shijima;

//pseudocode
std::string ReadFile(std::string const& path);
void SleepForMilliseconds(int ms);

/* ... */

// Create a mascot factory
mascot::factory factory;
factory.script_ctx = std::make_shared<scripting::context>();
factory.env = std::make_shared<mascot::environment>();

// Register mascot templates
mascot::factory::tmpl tmpl;
tmpl.name = "my_mascot";
tmpl.actions_xml = read_file("/path/to/shimeji/actions.xml");
tmpl.behaviors_xml = read_file("/path/to/shimeji/behaviors.xml");
tmpl.path = "/path/to/shimeji";
factory.register_template(tmpl);

// Spawn shimeji
mascot::manager::initializer init;
init.anchor.x = 100;
init.anchor.y = 100;
mascot::factory::product product = factory.spawn("my_mascot", init);
mascot::manager &shimeji = *product.manager;

// Run loop
while (true) {
    // Populate shimeji environment
    mascot::environment &env = *factory.env;
    const int w=500, h=500;
    env.work_area = env.screen = { 0, w, h, 0 };
    env.floor = { h-50, 0, w };
    env.ceiling = { 0, 0, w };

    // Tick
    shimeji.tick();

    // Render shimeji
    /*... platform dependent ...*/

    // Shimeji are designed to run at 25 FPS
    SleepForMilliseconds(1000 / 25);
}
```