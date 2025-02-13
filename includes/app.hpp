#ifndef _APP_HPP
#define _APP_HPP

#include<string>
#include<vector>
#include<stdio.h>
#include<stdbool.h>
#include<SDL2/SDL.h>

#define print(...) printf("[%s]: ", __func__); printf(__VA_ARGS__);

class App {
    public:
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool running;


        void tick_process_events();
        void tick_update(double dt);
        void tick_render();

        App(std::string title, int width, int height);
        ~App();
};

#endif // _APP_HPP