#include <SDL2/SDL.h>

#include <iostream>

#include "emulator.hpp"

#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

int main(int argc, char **argv) {
    
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) return -1;

    SDL_Window *window = SDL_CreateWindow(
        "cloNES",                 // window title
        SDL_WINDOWPOS_CENTERED,   // initial x position
        SDL_WINDOWPOS_CENTERED,   // initial y position
        512,                      // width, in pixels
        480,                      // height, in pixels
        SDL_WINDOW_SHOWN          // flags - see below
    );
    if (!window) return -1;

    bool fullscreen = false;
    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }

    bool headlessMode = false;
    // We create a renderer with hardware acceleration, 
    // we also present according with the vertical sync refresh.
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | ((headlessMode) ? 0 : SDL_RENDERER_PRESENTVSYNC)
    );
    if(!renderer) return -1;

    // Initialize emulator. 
    std::string romPath = argv[1];
    auto emulator = new cloNES::emulator(romPath);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STATIC, 
        256, 240
    );
    if(!texture) return -1;

    SDL_Event event;
    bool isRunning = true;
    Uint32 frameStart = SDL_GetTicks();
    while (isRunning) {
        emulator->step();
        if(!emulator->getPpuFlag()) continue;

        //Poll controller
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    isRunning = emulator->keyDown(event.key.keysym.sym);
                    break;
                case SDL_KEYUP:
                    isRunning = emulator->keyUp(event.key.keysym.sym);
                    break;
                default: 
                    break;
            }
        }

        //Draw frame
        emulator->setPpuFlag(false);
        SDL_UpdateTexture(texture, NULL, emulator->getPpuBuffer(), 256 * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Lock fps. 
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
        frameStart = SDL_GetTicks();
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_Quit();

    return 0;
}
