#ifndef UPDATEKEYS_H
#define UPDATEKEYS_H

#include <stdint.h>

uint16_t updateKeyDown(SDL_Event e, uint16_t k) {

    switch(e.key.keysym.sym) {
        case SDLK_z:
            k |= 0x01;
            break;
        case SDLK_x:
            k |= 0x02;
            break;
        case SDLK_c:
            k |= 0x04;
            break;
        case SDLK_v:
            k |= 0x08;
            break;
        case SDLK_a:
            k |= 0x10;
            break;
        case SDLK_s:
            k |= 0x20;
            break;
        case SDLK_d:
            k |= 0x40;
            break;
        case SDLK_f:
            k |= 0x80;
            break;
        case SDLK_q:
            k |= 0x100;
            break;
        case SDLK_w:
            k |= 0x200;
            break;
        case SDLK_e:
            k |= 0x400;
            break;
        case SDLK_r:
            k |= 0x800;
            break;
        case SDLK_1:
            k |= 0x1000;
            break;
        case SDLK_2:
            k |= 0x2000;
            break;
        case SDLK_3:
            k |= 0x4000;
            break;
        case SDLK_4:
            k |= 0x8000;
            break;
    }
    return k;
}

uint16_t updateKeyUp(SDL_Event e, uint16_t k) {

    switch(e.key.keysym.sym) {
        
        case SDLK_z:
            k &= 0xFFFF - 0x01;
            break;
        case SDLK_x:
            k &= 0xFFFF - 0x02;
            break;
        case SDLK_c:
            k &= 0xFFFF - 0x04;
            break;
        case SDLK_v:
            k &= 0xFFFF - 0x08;
            break;
        case SDLK_a:
            k &= 0xFFFF - 0x10;
            break;
        case SDLK_s:
            k &= 0xFFFF - 0x20;
            break;
        case SDLK_d:
            k &= 0xFFFF - 0x40;
            break;
        case SDLK_f:
            k &= 0xFFFF - 0x80;
            break;
        case SDLK_q:
            k &= 0xFFFF - 0x100;
            break;
        case SDLK_w:
            k &= 0xFFFF - 0x200;
            break;
        case SDLK_e:
            k &= 0xFFFF - 0x400;
            break;
        case SDLK_r:
            k &= 0xFFFF - 0x800;
            break;
        case SDLK_1:
            k &= 0xFFFF - 0x1000;
            break;
        case SDLK_2:
            k &= 0xFFFF - 0x2000;
            break;
        case SDLK_3:
            k &= 0xFFFF - 0x4000;
            break;
        case SDLK_4:
            k &= 0xFFFF - 0x8000;
            break;
    }
    return k;
}

#endif