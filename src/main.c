#include<stdio.h>
#include<stdbool.h>
#include<malloc.h>
#include<assert.h>
#include<SDL2/SDL.h>

#include<app.h>

app_t* app;

void game_handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: 
                print("Received SDL_QUIT signal.\n");
                app->running = false;
                break;
            default:
                break;
        }
    }
}
void game_update(double dt) {}
void game_render() {}

int main(int argc, char** argv) {
    app = (app_t*)malloc(sizeof(app_t));
    app->running = true;
    app->screen_width = 800;
    app->screen_height = 600;

    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
    app->window = SDL_CreateWindow(
        "Minesweeper", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        app->screen_width, app->screen_height, 
        0
    );
    assert(app->window != NULL);
    app->renderer = SDL_CreateRenderer(app->window, -1, 0);
    assert(app->renderer != NULL);
    print("Initialized application window and renderer.\n");

    double last_tick = (double)SDL_GetTicks();
    double current_tick = (double)SDL_GetTicks();
    double delta_accum = 0.0;
    double frametime = 1.0 / 50.0;

    print("Entering the mainloop.\n");
    while (app->running) {
        current_tick = (double)SDL_GetTicks();
        delta_accum += (current_tick - last_tick) / 1000.0;
        last_tick = current_tick;
        game_handle_events();
        while (delta_accum >= frametime) {
            delta_accum -= frametime;
            game_update(frametime);
        }
        game_render();
    }

    return 0;
}