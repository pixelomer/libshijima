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

#include <shijima/shijima.hpp>
#include <sstream>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <cassert>
#include <iostream>

#define WINDOW_TITLE "Shijima"

using namespace shijima;

bool enable_ie = false;

mascot::factory factory;

#if !defined(SHIJIMA_NO_PUGIXML)
typedef shijima::mascot::factory::tmpl shimeji_meta;
#else
typedef shijima::mascot::factory::registered_tmpl shimeji_meta;
#endif

struct shimeji_data {
    mascot::manager manager;
    std::shared_ptr<shimeji_meta> meta;
};

std::string read_file(std::string path) {
    std::stringstream buf;
    std::ifstream f(path, std::ios::in | std::ios::binary);
    buf << f.rdbuf();
    return buf.str();
}

shimeji_meta load_meta(std::string path) {
    auto name = path.substr(path.find_last_of('/')+1);
    #if !defined(SHIJIMA_NO_PUGIXML)
        auto actions = read_file(path + "/actions.xml");
        auto behaviors = read_file(path + "/behaviors.xml");
        return { name, actions, behaviors, path };
    #else
        auto serialized = read_file(path + "/mascot.cereal");
        return { name, serialized, path };
    #endif
}

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *window_surface;

std::map<std::string, SDL_Texture *> loaded_images;
SDL_Texture *get_image(std::string const& path) {
    if (loaded_images.count(path) == 1) {
        return loaded_images.at(path);
    }
    SDL_Surface *loaded = IMG_Load(path.c_str());
    assert(loaded != NULL);
    
    //FIXME: SDL_ConvertSurface() causes transparency to be lost on some systems (?)
    SDL_Surface *optimized = loaded;
    /*
    SDL_Surface *optimized = SDL_ConvertSurface(loaded, window_surface->format, 0);
    assert(optimized != NULL);
    SDL_FreeSurface(loaded);
    */
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, optimized);
    assert(texture != NULL);
    SDL_FreeSurface(optimized);
    loaded_images[path] = texture;
    return texture;
}

std::vector<mascot::factory::product> mascots;

void run_console(bool poll_events = true) {
    scripting::context &ctx = *mascots[0].manager->script_ctx;
    ctx.state = mascots[0].manager->state;
    ctx.state->env = factory.env;
    std::string line;
    do {
        std::cout << "duktape> ";
        std::getline(std::cin, line);
        SDL_Event ev;
        if (line == "q") {}
        else if (line == "tick") {
            mascots[0].manager->tick();
        }
        else {
            try {
                std::cout << ctx.eval_json(line) << std::endl;
            }
            catch (std::exception& err) {
                std::cerr << "error: " << err.what() << std::endl;
            }
        }
        if (poll_events) {
            while (SDL_PollEvent(&ev)); // ignore all events
        }
    }
    while (line != "q");
}

bool running = true;
bool spawn_more = false;
bool paused = false;
bool fast = false;
double active_ie_x = 400;
double active_ie_direction = 1;

Uint32 timer_callback(Uint32 interval, void *param) {
    (void)param; // unused
    (void)interval; // unused
    SDL_Event event;
    bzero(&event, sizeof(event));
    event.type = SDL_USEREVENT;

    SDL_PushEvent(&event);
    if (fast) {
        return 1000 / 300; // 3x
    }
    return 1000 / 100;
}

void tick() {
    // Update
    uint32_t start_time = SDL_GetTicks();

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    auto &env = *factory.env;
    env.subtick_count = 4;
    env.work_area = env.screen = { 0, (double)w, (double)h, 0 };
    env.floor = { (double)h - 50, 0, (double)w };
    env.ceiling = { 0, 0, (double)w };
    if (enable_ie) {
        double dx = active_ie_direction * (3 + random() % 5);
        active_ie_x += dx;
        env.active_ie = { 400, (double)active_ie_x + 300, 700,
            (double)active_ie_x };
        env.active_ie.dx = dx;
        env.active_ie.dy = 0;
        if (active_ie_x > 600) active_ie_direction = -1;
        if (active_ie_x < 200) active_ie_direction = 1;
    }
    else {
        // {0,0,0,0} may cause shimeji to get stuck on the top left corner
        env.active_ie = { -50, 50, -50, 50 };
    }
    auto old = env.cursor;
    env.cursor = { (double)mx, (double)my,
        (double)(mx - old.x), (double)(my - old.y) };

    // Set scale once env has been initialized
    env.set_scale(1.01);

    std::vector<mascot::factory::product> new_mascots;

    for (auto &mascot : mascots) {
        auto &manager = mascot.manager;
        manager->tick();
        auto &breed_request = manager->state->breed_request;
        if (breed_request.available) {
            breed_request.available = false;
            if (breed_request.name == "") {
                breed_request.name = mascot.tmpl->name;
            }
            try {
                auto product = factory.spawn(breed_request.name,
                    { breed_request.anchor, breed_request.behavior });
                product.manager->state->looking_right = breed_request.looking_right;
                new_mascots.push_back(std::move(product));
            }
            catch (...) {}
        }
    }

    // Reset scale after env has been used, then use anchors normally
    // (manager->state->anchor will always have scale 1.0 after tick)
    env.reset_scale();

    uint32_t update_elapsed = SDL_GetTicks() - start_time;
    start_time = SDL_GetTicks();

    // Render
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    if (enable_ie) {
        SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
        SDL_Rect rect = { (int)env.active_ie.left,
            (int)env.active_ie.top,
            (int)env.active_ie.width(),
            (int)env.active_ie.height() };
        SDL_RenderFillRect(renderer, &rect);
    }
    for (auto &product : mascots) {
        auto &manager = product.manager;
        SDL_Texture *texture = get_image(product.tmpl->path + "/img" +
            manager->state->active_frame.name);
        SDL_Rect src = {}, dest;
        SDL_QueryTexture(texture, NULL, NULL, &src.w, &src.h);
        if (manager->state->looking_right) {
            dest = {
                (int)(manager->state->anchor.x - src.w + manager->state->active_frame.anchor.x),
                (int)(manager->state->anchor.y - manager->state->active_frame.anchor.y),
                src.w, src.h };
        }
        else {
            dest = {
                (int)(manager->state->anchor.x - manager->state->active_frame.anchor.x),
                (int)(manager->state->anchor.y - manager->state->active_frame.anchor.y),
                src.w, src.h };
        }
        SDL_RenderCopyEx(renderer, texture, &src, &dest, 0.0, NULL,
            manager->state->looking_right ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
    SDL_RenderPresent(renderer);

    uint32_t render_elapsed = SDL_GetTicks() - start_time;

    if (spawn_more) {
        std::vector<std::string> names = {
            "test1"
        };
        auto name = names[random() % names.size()];
        math::vec2 anchor { static_cast<double>(50 + random() % (w - 100)),
            static_cast<double>(50 + random() % (h - 100)) };
        mascots.push_back(factory.spawn(name, { anchor }));
        spawn_more = false;
    }

    //if (mascots[0].manager->state->time % 12 == 0) {
        std::string title = WINDOW_TITLE " (" + std::to_string(mascots.size()) + " active, "
            + std::to_string(update_elapsed) + "ms update, "
            + std::to_string(render_elapsed) + "ms render)";
        
        SDL_SetWindowTitle(window, title.c_str());
    //}

    for (int i=new_mascots.size()-1; i>=0; --i) {
        mascots.push_back(std::move(new_mascots[i]));
        new_mascots.erase(new_mascots.begin() + i);
    }
}

bool handle_event(SDL_Event event) {
    switch (event.type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_c:
                    running = false;
                    run_console();
                    running = true;
                    break;
                case SDLK_a:
                    enable_ie = !enable_ie;
                    break;
                case SDLK_p:
                    paused = !paused;
                    break;
                case SDLK_m:
                    spawn_more = !spawn_more;
                    break;
                case SDLK_f:
                    fast = !fast;
                    break;
                case SDLK_SPACE:
                    if (paused) tick();
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            mascots[random() % mascots.size()].manager->state->dragging = true;
            break;
        case SDL_MOUSEBUTTONUP:
            for (auto &mascot : mascots) {
                mascot.manager->state->dragging = false;
            }
            break;
        case SDL_USEREVENT: {
            if (!paused) tick();
            break;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    #ifdef SHIJIMA_LOGGING_ENABLED
        shijima::set_log_level(SHIJIMA_LOG_EVERYTHING);
    #endif
    bool do_run_console = false;

    if (argc == 2 && strcmp(argv[1], "console") == 0) {
        do_run_console = true;
    }
    #if !defined(SHIJIMA_NO_PUGIXML)
    else if (argc == 2 && strcmp(argv[1], "translate") == 0) {
        std::stringstream buf;
        buf << std::cin.rdbuf();
        std::string input = buf.str();
        std::string output = shijima::translator::translate(input);
        std::cout << output;
        std::cout.flush();
        return EXIT_SUCCESS;
    }
    else if ((argc == 5 || argc == 4) && strcmp(argv[1], "serialize") == 0) {
        std::string actions, behaviors;
        std::string out_path;
        if (argc == 5) {
            actions = read_file(argv[2]);
            behaviors = read_file(argv[3]);
            out_path = argv[4];
        }
        else /* if (argc == 4) */ {
            actions = read_file(std::string(argv[2]) + "/actions.xml");
            behaviors = read_file(std::string(argv[2]) + "/behaviors.xml");
            out_path = argv[3];
        }
        shijima::parser parser;
        bool parsed = true;
        try {
            parser.parse(actions, behaviors);
        }
        catch (std::exception &err) {
            parsed = false;
            std::cerr << "=== shijima::parser failed ===" << std::endl;
            std::cerr << err.what() << std::endl;
        }
        if (parser.get_warnings().size() > 0) {
            std::cerr << "=== shijima::parser produced warnings ===" << std::endl;
        }
        for (auto const& warn : parser.get_warnings()) {
            std::cerr << warn.what() << std::endl;
        }
        if (!parsed) {
            return EXIT_FAILURE;
        }
        std::ofstream out;
        out.open(out_path, std::ios::out | std::ios::binary);
        parser.saveTo(out);
        out.close();
        return EXIT_SUCCESS;
    }
    #endif
    else {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "    " << argv[0] << std::endl;
        std::cerr << "    " << argv[0] << " console" << std::endl;
        #if !defined(SHIJIMA_NO_PUGIXML)
        std::cerr << "    " << argv[0] << " translate" << std::endl;
        std::cerr << "    " << argv[0] << " serialize <actions.xml> <behaviors.xml> "
            "<mascot.cereal>" << std::endl;
        std::cerr << "    " << argv[0] << " serialize <MyMascot.mascot> <mascot.cereal>"
            << std::endl;
        #endif
        return EXIT_FAILURE;
    }

    factory.script_ctx = std::make_shared<scripting::context>();
    factory.env = std::make_shared<mascot::environment>();
    static const std::vector<std::string> paths = {
        "tests/shimeji/test1"
    };
    for (auto &path : paths) {
        auto meta = load_meta(path);
        auto tmpl = factory.register_template(meta);
        #if !defined(SHIJIMA_NO_PUGIXML)
        std::ofstream out;
        out.open(path + "/mascot.cereal", std::ios::out | std::ios::binary);
        out << tmpl->data;
        out.close();
        #endif
    }
    math::vec2 anchor { 100, 100 };
    mascots.push_back(factory.spawn("test1", { anchor }));

    if (do_run_console) {
        run_console(false);
        return EXIT_SUCCESS;
    }

    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        std::cerr << "SDL_Init() failed: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    if (!(IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init() failed: " << IMG_GetError() << std::endl;
        return EXIT_FAILURE;
    }
        
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1024, 768,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    window_surface = SDL_GetWindowSurface(window);

    SDL_AddTimer(1000 / 100, timer_callback, NULL);
    running = true;
    spawn_more = false;

    while (true) {
        SDL_Event event;
        while (SDL_WaitEvent(&event)) {
            if (!handle_event(event)) {
                goto out;
            }
        }
    }
    out:

    SDL_DestroyWindow(window);
    SDL_Quit();
}
