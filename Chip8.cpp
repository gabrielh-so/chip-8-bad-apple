#include "include/Chip8.h"

Chip8::Chip8() {
    std::srand(std::time(NULL));

    PCreset = 200;
}

Chip8::~Chip8() {
    display = NULL;
}