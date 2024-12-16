#include <string>

#include <SDL2/SDL.h>

#include "include/6502.hpp"
#include "include/Controller.hpp"
#include "include/Mapper/Mapper.hpp"
#include "include/PPU.hpp"
#include "include/RAM.hpp"
#include "include/ROM.hpp"

namespace cloNES {
    
    class emulator {
    private: 
        MedNES::CPU6502    *cpu        = nullptr;
        MedNES::RAM        *ram        = nullptr;
        MedNES::ROM        *rom        = nullptr;
        MedNES::Mapper     *mapper     = nullptr;
        MedNES::PPU        *ppu        = nullptr;
        MedNES::Controller *controller = nullptr;
    public:
        emulator(std::string romPath);
        ~emulator();

        void step();

        bool getPpuFlag();
        void setPpuFlag(bool flag);

        uint32_t *getPpuBuffer();

        bool keyDown(SDL_Keycode keyCode);
        bool keyUp(SDL_Keycode keyCode);
    };

} // namespace cloNES
