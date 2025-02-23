#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<malloc.h>
#include<assert.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include<app.h>
#include<mineswep.h>

app_t* app;
minesweeper_t* game;
SDL_Texture* game_textures[13];

const int shift_x = 0;
const int shift_y = 0;

int start_grid_w = 25;
int start_grid_h = 25;
int start_mine_count = 49;

enum TextureIds {
    CELL_UNTOUCHED = 0,
    CELL_FLAGGED = 1,
    CELL_BOMB = 2,
    CELL_0 = 3,
    CELL_1 = 4,
    CELL_2 = 5,
    CELL_3 = 6,
    CELL_4 = 7,
    CELL_5 = 8,
    CELL_6 = 9,
    CELL_7 = 10,
    CELL_10 = 11,
    CELL_UNTOUCHED_HOVERED = 12
};

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
        
        case SDLK_g:
            if (!app->game_ended) break;
            for (int y = 0; y < game->grid_height; y++) {
                for (int x = 0; x < game->grid_width; x++) {
                    cell_t* cell = &game->cells[x][y];
                    cell->is_flagged = false;
                    cell->is_revealed = false;
                }
            }
            app->game_lost = false;
            app->game_ended = false;
            break;
        
        case SDLK_f:
            if (!app->game_ended) break;
            minesweeper_destroy(game);
            game = minesweeper_create(start_grid_w, start_grid_h, start_mine_count);
            app->game_lost = false;
            app->game_ended = false;
            break;
        
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
            app->cell_size -= 2;
            if (app->cell_size < 0) app->cell_size = 20;
            break;
        
        case SDLK_PLUS:
        case SDLK_KP_PLUS:
            app->cell_size += 2;
            break;
        
        default:
            break;
    }
}

void on_mouse_move(SDL_Event e) {
    int nmx = (int)e.motion.x;
    int nmy = (int)e.motion.y;

    if (app->mouse_active) {
        app->grid_moving = true;
        app->render_x -= (double)(nmx - app->mouse_x);
        app->render_y -= (double)(nmy - app->mouse_y);
    }

    app->mouse_x = nmx;
    app->mouse_y = nmy;
}

void check_if_won() {
    if (app->game_ended) return;

    int total = game->grid_height * game->grid_width;
    int revealed = 0;
    int total_mines = 0;

    for (int y = 0; y < game->grid_height; y++) {
        for (int x = 0; x < game->grid_width; x++) {
            cell_t cell = game->cells[x][y];
            if (cell.is_mine) total_mines += 1;
            if (cell.is_revealed) revealed += 1;
        }
    }

    if (total - total_mines == revealed) {
        app->game_ended = true;
        app->game_lost = false;
    }
}


void reveal_cell(int i, int j) {
    int width = game->grid_width;
    int height = game->grid_height;

    // I don't want to deal with constantly making if statements so just do it here.
    if (i < 0) return;
    if (j < 0) return;
    if (i >= width) return;
    if (j >= height) return;

    cell_t** cells = game->cells;
    cell_t* cell = &cells[i][j];

    if (cell->is_revealed) return; // End case for the recursion.
    if (cell->is_flagged) return;
    cell->is_revealed = true;

    if (cell->is_mine) {
        app->game_ended = true;
        app->game_lost = true;
    } else {
        check_if_won();
        if (cell->number != 0) return;
        reveal_cell(i - 1, j + 1);
        reveal_cell(i - 1, j    );
        reveal_cell(i - 1, j - 1);
        
        reveal_cell(i    , j + 1);
        reveal_cell(i    , j - 1);

        reveal_cell(i + 1, j + 1);
        reveal_cell(i + 1, j    );
        reveal_cell(i + 1, j - 1);
    }
}

void flag_cell(int i, int j) {
    cell_t* cell = &game->cells[i][j];
    if (cell->is_revealed) return;
    cell->is_flagged = !cell->is_flagged;
}

void on_mouse_release(SDL_Event e) {
    app->mouse_active = false;
    if (app->grid_moving) {
        app->grid_moving = false;
        return;
    };
    if (app->game_ended) return;
    int target_x = e.button.x;
    int target_y = e.button.y;
    int cell_x = target_x + (int)(app->render_x) - shift_x;
    int cell_y = target_y + (int)(app->render_y) - shift_y;
    int cell_i = cell_x / app->cell_size;
    int cell_j = cell_y / app->cell_size;

    if (cell_i < 0) return;
    if (cell_j < 0) return;
    if (cell_i >= game->grid_width) return;
    if (cell_j >= game->grid_height) return;

    if (e.button.button == SDL_BUTTON_LEFT) {
        reveal_cell(cell_i, cell_j);
    } else {
        flag_cell(cell_i, cell_j);
    }
}

void on_mouse_click(SDL_Event e) {
    app->mouse_active = true;
    app->grid_moving = false;
}

void handle_mouse_scroll(SDL_Event e) {
    app->cell_size += e.wheel.y;
    while (app->cell_size < 0) {
        app->cell_size = 20;
    }
}

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
            
            case SDL_MOUSEBUTTONUP:
                on_mouse_release(event);
                break;
            
            case SDL_MOUSEMOTION:
                on_mouse_move(event);
                break;
            
            case SDL_MOUSEWHEEL:
                handle_mouse_scroll(event);
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
        x - rx + shift_x, 
        y - ry + shift_y, 
        w,
        h
    };
    
    cell_t cell = game->cells[i][j];
    SDL_Texture* tex;
    if (cell.is_flagged) {
        tex = game_textures[CELL_FLAGGED];
    } else if (!cell.is_revealed) {
        tex = game_textures[CELL_UNTOUCHED];
        int hovered_x = app->mouse_x + (int)app->render_x - shift_x;
        int hovered_y = app->mouse_y + (int)app->render_y - shift_y;

        if ((x < hovered_x) && (hovered_x < x + w) && (y < hovered_y) && (hovered_y < y + h)) {
            tex = game_textures[CELL_UNTOUCHED_HOVERED];
        }

    } else if (cell.is_mine) {
        tex = game_textures[CELL_BOMB];
    } else {
        tex = game_textures[CELL_0 + cell.number];
    }
    SDL_RenderCopy(app->renderer, tex, NULL, &rect);
}

void app_render_text(char* text, SDL_Color fg, SDL_Color bg, int x, int y, int w, int h) {
    SDL_Surface* surf = TTF_RenderText(
        app->font,
        text,
        fg,
        bg
    );
    SDL_Texture* tex = SDL_CreateTextureFromSurface(app->renderer, surf);
    SDL_RenderCopy(
        app->renderer, 
        tex, 
        NULL, 
        &(SDL_Rect){
            .x = x,
            .y = y,
            .w = w,
            .h = h
        }
    );
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

void game_render() {
    SDL_SetRenderDrawColor(app->renderer, 255, 200, 200, 255);
    SDL_RenderClear(app->renderer);

    for (int y = 0; y < game->grid_height; y++) {
        for (int x = 0; x < game->grid_width; x++) {
            game_render_cell(x, y);
        }
    }

    if (app->game_ended) {
        SDL_Color fg = (SDL_Color){.r = 255, .b = 200, .g = 200, .a = 255};
        SDL_Color bg = (SDL_Color){.r = 0, .b = 0, .g =0, .a = 255};        

        if (app->game_lost) {
            app_render_text(
                "YOU LOST. CLICK G TO REPLAY SAME GRID. CLICK F TO GENERATE NEW GRID WITH SAME SETTINGS.", 
                fg, 
                bg, 
                0, app->screen_height-50,
                600, 30
            );
        } else {
            app_render_text(
                "YOU WON. CLICK G TO REPLAY SAME GRID. CLICK F TO GENERATE NEW GRID WITH SAME SETTINGS.", 
                fg, 
                bg, 
                0, app->screen_height-50,
                600, 30
            );
            
        }
    }

    SDL_RenderPresent(app->renderer);
}

int main(int argc, char** argv) {
    int mode = 0;
    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];
        if (mode == 0) {
            if (strcmp(arg, "-w") == 0) {
                mode = 1;
            }
            if (strcmp(arg, "-h") == 0) {
                mode = 2;
            }
            if (strcmp(arg, "-c") == 0) {
                mode = 3;
            }
        } else {
            if (mode == 1) start_grid_w = SDL_atoi(arg);
            else if (mode == 2) start_grid_h = SDL_atoi(arg);
            else start_mine_count = SDL_atoi(arg);
            mode = 0;
        }
    }


    srand(time(NULL));

    app = (app_t*)malloc(sizeof(app_t));
    game = minesweeper_create(start_grid_w, start_grid_h, start_mine_count);

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

    app->game_ended = false;
    app->game_lost = false;
    app->grid_moving = false;
    app->mouse_active = false;

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
    assert(SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND) == 0);
    print("Initialized application window and renderer.\n");
    assert(TTF_Init() == 0);

    game_textures[0] = IMG_LoadTexture(app->renderer, "./res/cell_untouched.png");
    game_textures[1] = IMG_LoadTexture(app->renderer, "./res/cell_flagged.png");
    game_textures[2] = IMG_LoadTexture(app->renderer, "./res/cell_bomb.png");
    game_textures[3] = IMG_LoadTexture(app->renderer, "./res/cell_0.png");
    game_textures[4] = IMG_LoadTexture(app->renderer, "./res/cell_1.png");
    game_textures[5] = IMG_LoadTexture(app->renderer, "./res/cell_2.png");
    game_textures[6] = IMG_LoadTexture(app->renderer, "./res/cell_3.png");
    game_textures[7] = IMG_LoadTexture(app->renderer, "./res/cell_4.png");
    game_textures[8] = IMG_LoadTexture(app->renderer, "./res/cell_5.png");
    game_textures[9] = IMG_LoadTexture(app->renderer, "./res/cell_6.png");
    game_textures[10] = IMG_LoadTexture(app->renderer, "./res/cell_7.png");
    game_textures[11] = IMG_LoadTexture(app->renderer, "./res/cell_8.png");
    game_textures[12] = IMG_LoadTexture(app->renderer, "./res/cell_untouched_hover.png");

    for (int i = 0; i < 13; i++) {
        if (game_textures[i] == NULL) {
            print("Texture %i is NULL.\n", i);
            exit(1);
        }
    }
    app->font = TTF_OpenFont("./OpenSans-Regular.ttf", 24);
    assert(app->font != NULL);
    
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