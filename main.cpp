#include <SDL2/SDL.h>

#include <iostream>

#include "include/6502.hpp"
#include "include/Controller.hpp"
#include "include/Mapper/Mapper.hpp"
#include "include/PPU.hpp"
#include "include/ROM.hpp"

#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

int main(int argc, char **argv) {
    
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        return 1; // SDL_Init failed. 
    }

    SDL_Window *window;
    std::string window_title = "cloNES";
    window = SDL_CreateWindow(
        window_title.c_str(),     // window title
        SDL_WINDOWPOS_CENTERED,   // initial x position
        SDL_WINDOWPOS_CENTERED,   // initial y position
        512,                      // width, in pixels
        480,                      // height, in pixels
        SDL_WINDOW_SHOWN          // flags - see below
    );

    if (window == NULL) {
        return 1; // SDL_CreateWindow failed. 
    }

    bool fullscreen = false;
    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }

    bool headlessMode = false;
    // We create a renderer with hardware acceleration, 
    // we also present according with the vertical sync refresh.
    SDL_Renderer *s = SDL_CreateRenderer(
        window, 
        0, 
        SDL_RENDERER_ACCELERATED | ((headlessMode) ? 0 : SDL_RENDERER_PRESENTVSYNC)
    );

    std::string romPath = argv[1];
    MedNES::ROM rom;
    rom.open(romPath);
    rom.printHeader();

    MedNES::Mapper *mapper = rom.getMapper();
    if (mapper == NULL) { 
        return 1; // Unknown mapper. 
    }

    auto ppu = MedNES::PPU(mapper);
    MedNES::Controller controller;
    auto cpu = MedNES::CPU6502(mapper, &ppu, &controller);
    cpu.reset();
    SDL_Texture *texture = SDL_CreateTexture(
        s, 
        SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STATIC, 
        256, 
        240
    );

    SDL_Event event;
    bool is_running = true;
    Uint32 frameStart = SDL_GetTicks();
    while (is_running) {
        cpu.step();
        if(!ppu.generateFrame) continue;

        //Poll controller
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    is_running = controller.setButtonPressed(event.key.keysym.sym, true);
                    break;
                case SDL_KEYUP:
                    is_running = controller.setButtonPressed(event.key.keysym.sym, false);
                    break;
                default: 
                    break;
            }
        }

        //Draw frame
        ppu.generateFrame = false;
        SDL_RenderSetScale(s, 2, 2);
        SDL_UpdateTexture(texture, NULL, ppu.buffer, 256 * sizeof(Uint32));
        SDL_RenderClear(s);
        SDL_RenderCopy(s, texture, NULL, NULL);
        SDL_RenderPresent(s);

        // Lock fps. 
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
        frameStart = SDL_GetTicks();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
