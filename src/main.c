#include<stdio.h>
#include<stdbool.h>
#include<malloc.h>
#include<assert.h>
#include<SDL2/SDL.h>

#include<app.h>
#include<mineswep.h>

app_t* app;
minesweeper_t* game;

void handle_keypress(SDL_Event event) {
    bool pressed = (event.type == SDL_KEYDOWN) ? true : false;

    switch (event.key.keysym.sym) {
        case SDLK_UP:
        case SDLK_w:
            app->grid_mot_y = pressed ? -1 : 0;
            break;

        case SDLK_DOWN:
        case SDLK_s:
            app->grid_mot_y = pressed ? 1 : 0;
            break;

        case SDLK_LEFT:
        case SDLK_a:
            app->grid_mot_x = pressed ? -1 : 0;
            break;

        case SDLK_RIGHT:
        case SDLK_d:
            app->grid_mot_x = pressed ? 1 : 0;
            break;

        default:
            break;
    }
}

void on_mouse_move(SDL_Event e) {
    app->mouse_x = (int)e.motion.x;
    app->mouse_y = (int)e.motion.y;
}
void on_mouse_click(SDL_Event e) {}

void game_handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: 
                print("Received SDL_QUIT signal.\n");
                app->running = false;
                break;
            
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                handle_keypress(event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                on_mouse_click(event);
                break;
            case SDL_MOUSEMOTION:
                on_mouse_move(event);
                break;

            default:
                break;
        }
    }
}
void game_update(double dt) {
    app->render_x += app->grid_mot_x * 100.0 * dt;
    app->render_y += app->grid_mot_y * 100.0 * dt;
}

void game_render_cell(int i, int j) {
    int x = i * app->cell_size;
    int y = j * app->cell_size;
    int w = app->cell_size;
    int h = app->cell_size;

    int rx = (int)(app->render_x);
    int ry = (int)(app->render_y);
    int sw = app->screen_width;
    int sh = app->screen_height;

    if ((x + w) < rx) return;
    if ((y + h) < ry) return;
    if (x > (rx + sw)) return;
    if (y > (ry + sh)) return;

    SDL_Rect rect = {
        x - rx, 
        y - ry, 
        w,
        h
    };
    
    if ((rect.x < app->mouse_x) && (app->mouse_x < (rect.x + rect.w)) && (rect.y < app->mouse_y) && (app->mouse_y < (rect.y + rect.h))) {
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    } else {
        SDL_SetRenderDrawColor(app->renderer, 150, 150, 200, 255);
    }

    SDL_RenderFillRect(app->renderer, &rect);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(app->renderer, &rect);
}

void game_render() {
    SDL_SetRenderDrawColor(app->renderer, 255, 200, 200, 255);
    SDL_RenderClear(app->renderer);

    for (int y = 0; y < game->grid_height; y++) {
        for (int x = 0; x < game->grid_width; x++) {
            game_render_cell(x, y);
        }
    }

    SDL_RenderPresent(app->renderer);
}

int main(int argc, char** argv) {
    app = (app_t*)malloc(sizeof(app_t));
    game = minesweeper_create(500, 400, 500*200);

    app->running = true;
    app->screen_width = 800;
    app->screen_height = 600;

    app->cell_size = 20;
    app->render_x = 0.0;
    app->render_y = 0.0;
    app->grid_mot_x = 0;
    app->grid_mot_y = 0;
    app->mouse_x = 0;
    app->mouse_y = 0;

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
            game_update(frametime);
            delta_accum -= frametime;
        }
        game_render();
    }

    return 0;
}