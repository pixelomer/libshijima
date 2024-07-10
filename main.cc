#include <shijima/shijima.hpp>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#define WINDOW_TITLE "Shijima"

using namespace shijima;

bool enable_ie = false;

mascot::factory factory;

struct shimeji_meta {
    std::string actions_xml;
    std::string behaviors_xml;
    std::string path;
    shimeji_meta(std::string actions_xml, std::string behaviors_xml,
        std::string path): actions_xml(actions_xml), behaviors_xml(behaviors_xml),
        path(path) {}
};

struct shimeji_data {
    mascot::manager manager;
    std::shared_ptr<shimeji_meta> meta;
};

shimeji_meta load_meta(std::string path) {
    std::string actions, behaviors;
    {
        std::stringstream buf;
        std::ifstream f(path + "/actions.xml");
        buf << f.rdbuf();
        actions = buf.str();
    }
    {   
        std::stringstream buf;
        std::ifstream f(path + "/behaviors.xml");
        buf << f.rdbuf();
        behaviors = buf.str();
    }
    return { actions, behaviors, path };
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
    while (true) {
        std::string line;
        std::cout << "duktape> ";
        std::getline(std::cin, line);
        SDL_Event ev;
        if (line != "q") {
            try {
                std::cout << ctx.eval_string(line) << std::endl;
            }
            catch (std::exception& err) {
                std::cerr << "error: " << err.what() << std::endl;
            }
        }
        if (poll_events) {
            while (SDL_PollEvent(&ev)); // ignore all events
        }
        if (line == "q") {
            break;
        }
    }
}

bool running = true;
bool spawn_more = false;
bool paused = false;
bool fast = false;

Uint32 timer_callback(Uint32 interval, void *param) {
    (void)param; // unused
    (void)interval; // unused
    SDL_Event event;
    bzero(&event, sizeof(event));
    event.type = SDL_USEREVENT;

    SDL_PushEvent(&event);
    if (fast) {
        return 1000 / 125;
    }
    else {
        return 1000 / 25;
    }
}

void tick() {
    // Update
    uint32_t start_time = SDL_GetTicks();

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    auto &env = *factory.env;
    env.work_area = env.screen = { 0, w, h, 0 };
    env.floor = { h-50, 0, w };
    env.ceiling = { 0, 0, w };
    if (enable_ie) {
        env.active_ie = { 400, 700, 700, 400 };
    }
    else {
        // {0, 0} may cause shimeji to get stuck on the top left corner
        env.active_ie = { 50, -1000, 0, 0 };
    }
    auto old = env.cursor;
    env.cursor = { (double)mx, (double)my,
        (double)(mx - old.x), (double)(my - old.y) };

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
            auto product = factory.spawn(breed_request.name,
                { breed_request.anchor, breed_request.behavior });
            product.manager->state->looking_right = breed_request.looking_right;
            new_mascots.push_back(std::move(product));
        }
    }

    uint32_t update_elapsed = SDL_GetTicks() - start_time;
    start_time = SDL_GetTicks();

    // Render
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    if (enable_ie) {
        SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
        SDL_Rect rect = { .x = env.active_ie.left,
            .y = env.active_ie.top,
            .w = env.active_ie.width(),
            .h = env.active_ie.height() };
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
                .x = (int)(manager->state->anchor.x - src.w + manager->state->active_frame.anchor.x),
                .y = (int)(manager->state->anchor.y - manager->state->active_frame.anchor.y),
                .w = src.w, .h = src.h };
        }
        else {
            dest = {
                .x = (int)(manager->state->anchor.x - manager->state->active_frame.anchor.x),
                .y = (int)(manager->state->anchor.y - manager->state->active_frame.anchor.y),
                .w = src.w, .h = src.h };
        }
        SDL_RenderCopyEx(renderer, texture, &src, &dest, 0.0, NULL,
            manager->state->looking_right ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
    SDL_RenderPresent(renderer);

    uint32_t render_elapsed = SDL_GetTicks() - start_time;

    if (spawn_more) {
        if (mascots[0].manager->state->time % 1 == 0) {
            std::vector<std::string> names = {
                "test1"
            };
            auto name = names[random() % names.size()];
            mascots.push_back(factory.spawn(name, { {
                (double)(50 + random() % (w - 100)),
                (double)(50 + random() % (h - 100)) } }));
        }
    }

    if (mascots[0].manager->state->time % 12 == 0) {
        std::string title = WINDOW_TITLE " (" + std::to_string(mascots.size()) + " active, "
            + std::to_string(update_elapsed) + "ms update, "
            + std::to_string(render_elapsed) + "ms render)";
        
        SDL_SetWindowTitle(window, title.c_str());
    }

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
    shijima::set_log_level(SHIJIMA_LOG_EVERYTHING);
    bool do_run_console = false;

    if (argc > 2) {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "    " << argv[0] << std::endl;
        std::cerr << "    " << argv[0] << " console" << std::endl;
        std::cerr << "    " << argv[0] << " translate" << std::endl;
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        if (strcmp(argv[1], "console") == 0) {
            do_run_console = true;
        }
        else if (strcmp(argv[1], "translate") == 0) {
            std::stringstream buf;
            buf << std::cin.rdbuf();
            std::string input = buf.str();
            std::string output = shijima::translator::translate(input);
            std::cout << output;
            std::cout.flush();
            return EXIT_SUCCESS;
        }
    }

    factory.script_ctx = std::make_shared<scripting::context>();
    factory.env = std::make_shared<mascot::environment>();
    static const std::vector<std::string> paths = {
        "tests/shimeji/test1"
    };
    for (auto &path : paths) {
        auto meta = load_meta(path);
        mascot::factory::tmpl tmpl;
        tmpl.name = path.substr(path.find_last_of('/')+1);
        tmpl.actions_xml = meta.actions_xml;
        tmpl.behaviors_xml = meta.behaviors_xml;
        tmpl.path = path;
        factory.register_template(tmpl);
    }
    mascots.push_back(factory.spawn("test1", { { 100, 100 } }));

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
        800, 600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    window_surface = SDL_GetWindowSurface(window);

    SDL_AddTimer(1000/25, timer_callback, NULL);
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