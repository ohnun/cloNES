#include "../include/Controller.hpp"

namespace MedNES {

u8 Controller::read(u16 address) {
    if (address == 0x4016) {
        if (strobe) {
            return 0x40 | (btnState & 1);
        }

        JOY1 = 0x80 | (btnStateLocked & 1);
        btnStateLocked >>= 1;
        return JOY1;
    } else {
        //TODO: Implement JOY2
        return JOY2;
    }
}

void Controller::write(u16 address, u8 data) {
    if (address == 0x4016) {
        if (strobe && !(data & 0x1)) {
            btnStateLocked = btnState;
        }

        strobe = data & 0x1;
    } else {
        //TODO: Implement JOY2
    }
}

bool Controller::setButtonPressed(SDL_Keycode key, bool pressed) {
    switch (key) {
        case SDLK_j: // button A
            btnState = (pressed) ? (btnState | (1 << 0)) : (btnState & ~(1 << 0));
            return true;

        case SDLK_k: // button B
            btnState = (pressed) ? (btnState | (1 << 1)) : (btnState & ~(1 << 1));
            return true;

        case SDLK_q: // Select
            btnState = (pressed) ? (btnState | (1 << 2)) : (btnState & ~(1 << 2));
            return true;

        case SDLK_e: // Start
            btnState = (pressed) ? (btnState | (1 << 3)) : (btnState & ~(1 << 3));
            return true;

        case SDLK_w: // UP
            btnState = (pressed) ? (btnState | (1 << 4)) : (btnState & ~(1 << 4));
            return true;

        case SDLK_s: // DOWN
            btnState = (pressed) ? (btnState | (1 << 5)) : (btnState & ~(1 << 5));
            return true;

        case SDLK_a: // LEFT
            btnState = (pressed) ? (btnState | (1 << 6)) : (btnState & ~(1 << 6));
            return true;

        case SDLK_d: // RIGHT
            btnState = (pressed) ? (btnState | (1 << 7)) : (btnState & ~(1 << 7));
            return true;

        case SDLK_x: // Exit
            return false;

        default:
            return true;
    }
}

}  //namespace MedNES
