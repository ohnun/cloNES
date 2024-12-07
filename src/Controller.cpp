#include "Controller.hpp"

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

void Controller::setButtonPressed(SDL_Keycode key, bool pressed) {
    if (key == SDLK_j) { // button A
        btnState = (pressed) ? (btnState | (1 << 0)) : (btnState & ~(1 << 0));
    }

    if (key == SDLK_k) { // button B
        btnState = (pressed) ? (btnState | (1 << 1)) : (btnState & ~(1 << 1));
    }

    if (key == SDLK_SPACE) { // Select
        btnState = (pressed) ? (btnState | (1 << 2)) : (btnState & ~(1 << 2));
    }

    if (key == SDLK_RETURN) { // Start
        btnState = (pressed) ? (btnState | (1 << 3)) : (btnState & ~(1 << 3));
    }

    if (key == SDLK_w) { // UP
        btnState = (pressed) ? (btnState | (1 << 4)) : (btnState & ~(1 << 4));
    }

    if (key == SDLK_s) { // DOWN
        btnState = (pressed) ? (btnState | (1 << 5)) : (btnState & ~(1 << 5));
    }

    if (key == SDLK_a) { // LEFT
        btnState = (pressed) ? (btnState | (1 << 6)) : (btnState & ~(1 << 6));
    }

    if (key == SDLK_d) { // RIGHT
        btnState = (pressed) ? (btnState | (1 << 7)) : (btnState & ~(1 << 7));
    }
}

}  //namespace MedNES
