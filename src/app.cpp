#include<string>
#include<stdexcept>
#include<app.hpp>

void _app_initialize_sdl(App* app, std::string title, int width, int height) {
    app->window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        0
    );
    if (app->window == NULL) {
        throw std::runtime_error("Failed to create SDL window (App:App)");
    }
    app->renderer = SDL_CreateRenderer(
        app->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if (app->renderer == NULL) {
        throw std::runtime_error("Failed to create SDL renderer (App:App)");
    }
    app->running = true;
}

App::App(std::string title, int width, int height) {
    _app_initialize_sdl(this, title, width, height);
    print("Initialized app object.\n");
}

App::~App() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    print("Destroyed app object.\n");
}

void App::tick_process_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            print("Received quit signal.\n");
            this->running = false;
            continue;
        }
    }
}

void App::tick_update(double dt) {
}

void App::tick_render() {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);

    SDL_RenderPresent(this->renderer);
}