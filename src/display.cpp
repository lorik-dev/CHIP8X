#include <iostream>
#include <iomanip>

#include "include/display.h"
#include "include/data.h"

std::shared_ptr<display> display::instance = nullptr;

display::display() {
    if (!display::init_sdl())
    {
        std::cout << "ERROR: Unable to initialise display. Check previous error messages. Exiting..." << std::endl;
        exit(-1);
    }
    std::cout << "Display initialised." << std::endl;
}

display::~display() {
    std::cout << "Display destroyed." << std::endl;
}

bool display::init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("ERROR: Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    // SDL_CreateWindow(title, x, y, w, h, flags)
    window_sdl = SDL_CreateWindow("CHIP8x", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, info::INTERNAL_SCREEN_WIDTH * info::SCALE_FACTOR, info::INTERNAL_SCREEN_HEIGHT * info::SCALE_FACTOR, 0);
    if (!window_sdl) {
        SDL_Log("ERROR: Could not create an SDL window %s\n", SDL_GetError());
        return false;
    }
    SDL_Log("SDL window created.\n");

    renderer_sdl = SDL_CreateRenderer(window_sdl, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_sdl) {
        SDL_Log("ERROR: Could not create an SDL renderer %s\n", SDL_GetError());
        return false;
    }
    SDL_Log("SDL renderer initialised.\n");

    return true; // Success
}

std::shared_ptr<display> display::getInstance() {
    if (instance == nullptr) {
        instance = std::shared_ptr<display>(new display());
    }
    return instance;
}