#include <shijima/shijima.hpp>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

using namespace shijima;

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
    SDL_Surface *optimized = SDL_ConvertSurface(loaded, window_surface->format, 0);
    assert(optimized != NULL);
    SDL_FreeSurface(loaded);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, optimized);
    assert(texture != NULL);
    SDL_FreeSurface(optimized);
    loaded_images[path] = texture;
    return texture;
}

std::vector<mascot::factory::product> mascots;

void run_console() {
    scripting::context &ctx = *mascots[0].manager->script_ctx;
    ctx.state = std::make_shared<mascot::state>();
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
        while (SDL_PollEvent(&ev)); // ignore all events
        if (line == "q") {
            break;
        }
    }
}

bool running;
bool spawn_more;

Uint32 timer_callback(Uint32 interval, void *param) {
    (void)param; // unused
    SDL_Event event;
    bzero(&event, sizeof(event));
    event.type = SDL_USEREVENT;

    SDL_PushEvent(&event);
    return interval;
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
                case SDLK_s:
                    spawn_more = !spawn_more;
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
            // Update
            uint32_t start_time = SDL_GetTicks();

            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            int mx, my;
            SDL_GetMouseState(&mx, &my);

            factory.env->work_area = factory.env->screen = { 0, w, h, 0 };
            factory.env->floor = { h-50, 0, w };
            factory.env->ceiling = { 0, 0, w };
            factory.env->active_ie = { 0, 0, 0, 0 };
            auto old = factory.env->cursor;
            factory.env->cursor = { (double)mx, (double)my,
                (double)(mx - old.x), (double)(my - old.y) };

            for (auto &mascot : mascots) {
                auto &manager = mascot.manager;
                manager->tick();
            }

            uint32_t update_elapsed = SDL_GetTicks() - start_time;
            start_time = SDL_GetTicks();

            // Render
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
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
                if (mascots[0].manager->state->time % 2 == 0) {
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
                std::string title = "Shimeji (" + std::to_string(mascots.size()) + " active, "
                    + std::to_string(update_elapsed) + "ms update, "
                    + std::to_string(render_elapsed) + "ms render)";
                
                SDL_SetWindowTitle(window, title.c_str());
            }

            break;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "console") == 0) {
        run_console();
        return EXIT_SUCCESS;
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

    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        std::cerr << "SDL_Init() failed: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    if (!(IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init() failed: " << IMG_GetError() << std::endl;
        return EXIT_FAILURE;
    }
        
    window = SDL_CreateWindow(
        "Shimeji",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 800,
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