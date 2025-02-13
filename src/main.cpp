#include<stdint.h>
#include<string>
#include<app.hpp>

int main(int argc, char** argv) {
    App app("2D Minesweeper Game", 800, 600);

    double frame_time = 0.02; // 1 / 50
    double accumulator = 0;
    double last_tick = (double)SDL_GetTicks() / 1000;
    double current_tick = (double)SDL_GetTicks() / 1000;

    while (app.running) {
        current_tick = (double)SDL_GetTicks() / 1000;
        accumulator += (current_tick - last_tick);
        last_tick = current_tick;

        while (accumulator >= frame_time) {
            app.tick_process_events();
            if (!app.running) 
                goto out_loop;
            app.tick_update(frame_time);
            accumulator -= frame_time;
        }
        app.tick_render();
    }
out_loop:
    return 0;
}