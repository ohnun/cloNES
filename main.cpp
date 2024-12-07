#include <SDL2/SDL.h>

#include <chrono>
#include <iostream>
#include <map>

#include "src/6502.hpp"
#include "src/Controller.hpp"
#include "src/Mapper/Mapper.hpp"
#include "src/PPU.hpp"
#include "src/ROM.hpp"

int main(int argc, char **argv) {
    std::string romPath = argv[1];

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        return 1; // SDL_Init failed. 
    }

    SDL_Window *window;
    std::string window_title = "cloNES";
    bool headlessMode = false;

    window = SDL_CreateWindow(
        window_title.c_str(),     // window title
        SDL_WINDOWPOS_UNDEFINED,  // initial x position
        SDL_WINDOWPOS_UNDEFINED,  // initial y position
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

    bool is_running = true;

    SDL_Event event;
    // We create a renderer with hardware acceleration, we also present according with the vertical sync refresh.
    SDL_Renderer *s = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | ((headlessMode) ? 0 : SDL_RENDERER_PRESENTVSYNC));

    MedNES::ROM rom;
    rom.open(romPath);
    rom.printHeader();
    MedNES::Mapper *mapper = rom.getMapper();

    if (mapper == NULL) {
        std::cout << "Unknown mapper.";
        return 1;
    }

    auto ppu = MedNES::PPU(mapper);
    MedNES::Controller controller;
    auto cpu = MedNES::CPU6502(mapper, &ppu, &controller);
    cpu.reset();
    SDL_Texture *texture = SDL_CreateTexture(s, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 256, 240);

    //For perf
    int nmiCounter = 0;
    float duration = 0;

    while (is_running) {
        cpu.step();

        if (ppu.generateFrame) {
            //Poll controller
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                        controller.setButtonPressed(event.key.keysym.sym, true);
                        break;
                    case SDL_KEYUP:
                        controller.setButtonPressed(event.key.keysym.sym, false);
                        break;
                    case SDL_QUIT:
                        is_running = false;
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
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
