#ifndef _APP_H
#define _APP_H

#include<stdio.h>
#include<stdbool.h>
#include<SDL2/SDL.h>

#define print(...) printf("[%s]: ", __func__); printf(__VA_ARGS__);

typedef struct app_t {
    bool running;
    int screen_width;
    int screen_height;
    SDL_Window* window;
    SDL_Renderer* renderer;
} app_t;

#endif // _APP_H