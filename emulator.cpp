#include "emulator.hpp"

namespace cloNES {

    emulator::emulator(std::string romPath) {
        ram = new MedNES::RAM();
        rom = new MedNES::ROM(romPath);
        mapper = rom->getMapper();
        ppu = new MedNES::PPU(mapper);
        controller = new MedNES::Controller();
        cpu = new cloNES::CPU(ram, mapper, ppu, controller);
        
        cpu->reset();
    }

    emulator::~emulator() {
        if(ram)        delete ram;
        if(rom)        delete rom;
        if(mapper)     delete mapper;
        if(ppu)        delete ppu;
        if(controller) delete controller;
        if(cpu)        delete cpu;
    }

    void emulator::step() {
        cpu->step();
    }

    bool emulator::getPpuFlag() {
        return ppu->generateFrame;
    }
    void emulator::setPpuFlag(bool flag) {
        ppu->generateFrame = flag;
    }

    uint32_t *emulator::getPpuBuffer() {
        return ppu->buffer;
    }

    bool emulator::keyDown(SDL_Keycode keyCode) {
        return controller->setButtonPressed(keyCode, true);
    }
    bool emulator::keyUp(SDL_Keycode keyCode) {
        return controller->setButtonPressed(keyCode, false);
    }
    
} // namespace cloNE 
