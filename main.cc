#include <shijima/shijima.hpp>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

using namespace shijima;

struct shimeji_data {
    std::string actions_xml;
    std::string behaviors_xml;
    std::string path;
};

shimeji_data load_shimeji(std::string path) {
    shimeji_data data;
    data.path = path;
    {
        std::stringstream buf;
        std::ifstream f(path + "/actions.xml");
        buf << f.rdbuf();
        data.actions_xml = buf.str();
    }
    {   
        std::stringstream buf;
        std::ifstream f(path + "/behaviors.xml");
        buf << f.rdbuf();
        data.behaviors_xml = buf.str();
    }
    return data;
}

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *window_surface;
shimeji_data shimeji;

std::map<std::string, SDL_Texture *> loaded_images;
SDL_Texture *get_image(std::string const& name) {
    if (loaded_images.count(name) == 1) {
        return loaded_images.at(name);
    }
    SDL_Surface *loaded = IMG_Load((shimeji.path + "/img" + name).c_str());
    assert(loaded != NULL);
    SDL_Surface *optimized = SDL_ConvertSurface(loaded, window_surface->format, 0);
    assert(optimized != NULL);
    SDL_FreeSurface(loaded);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, optimized);
    assert(texture != NULL);
    SDL_FreeSurface(optimized);
    loaded_images[name] = texture;
    return texture;
}

void run_console(std::shared_ptr<mascot::state> state) {
    auto ctx = scripting::context::get();
    ctx->state = state;
    while (true) {
        std::string line;
        std::cout << "duktape> ";
        std::getline(std::cin, line);
        if (line == "q") {
            break;
        }
        try {
            std::cout << ctx->eval_string(line) << std::endl;
        }
        catch (std::exception& err) {
            std::cerr << "error: " << err.what() << std::endl;
        }
    }
}

bool running;

Uint32 timer_callback(Uint32 interval, void *param) {
    SDL_Event event;
    bzero(&event, sizeof(event));
    event.type = SDL_USEREVENT;

    SDL_PushEvent(&event);
    return interval;
}

bool handle_event(SDL_Event event, mascot::manager &mascot) {
    switch (event.type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_c:
                    running = false;
                    run_console(mascot.state);
                    running = true;
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            mascot.state->dragging = true;
            break;
        case SDL_MOUSEBUTTONUP:
            mascot.state->dragging = false;
            break;
        case SDL_USEREVENT: {
            // Update
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            mascot.state->env.work_area = { 0, w, h, 0 };
            mascot.state->env.floor = { h-50, 0, w };
            mascot.state->env.ceiling = { 0, 0, w };
            mascot.state->env.active_ie = { 0, 0, 0, 0 };
            mascot.state->env.cursor = { (double)mx, (double)my };
            mascot.tick();

            // Render
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
            SDL_Texture *texture = get_image(mascot.state->active_frame.name);
            SDL_Rect src = {}, dest;
            SDL_QueryTexture(texture, NULL, NULL, &src.w, &src.h);
            dest = { .w = src.w, .h = src.h,
                .x = (int)(mascot.state->anchor.x - mascot.state->active_frame.anchor.x),
                .y = (int)(mascot.state->anchor.y - mascot.state->active_frame.anchor.y) };
            SDL_RenderCopy(renderer, texture, &src, &dest);

            SDL_RenderPresent(renderer);

            break;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "console") == 0) {
        run_console(std::make_shared<mascot::state>());
        return EXIT_SUCCESS;
    }

    shimeji = load_shimeji("tests/shimeji/test1");
    mascot::manager mascot(shimeji.actions_xml, shimeji.behaviors_xml);

    mascot.state->anchor = { 375, 400 };
    /*while (1) {
        tick(mascot);
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000000000LL / 25)); // 1/25 secs
    }*/

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
        800, 600,
        SDL_WINDOW_SHOWN
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    window_surface = SDL_GetWindowSurface(window);

    SDL_AddTimer(1000/25, timer_callback, NULL);
    running = true;

    while (true) {
        SDL_Event event;
        while (SDL_WaitEvent(&event)) {
            if (!handle_event(event, mascot)) {
                goto out;
            }
        }
    }
    out:

    SDL_DestroyWindow(window);
    SDL_Quit();
}