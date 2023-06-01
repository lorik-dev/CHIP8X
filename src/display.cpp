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

    texture_sdl = SDL_CreateTexture(renderer_sdl, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, info::INTERNAL_SCREEN_WIDTH,
        info::INTERNAL_SCREEN_HEIGHT);
    if (!texture_sdl) {
        std::cerr << "ERROR: Could not create an SDL texture %s\n" << SDL_GetError();
        return false;
    }
    SDL_Log("SDL texture initialized.\n");

    return true; // Success
}

// Update window with any changes
void display::update_screen(std::array<uint32_t, info::INTERNAL_SCREEN_PIXELS> &SCREEN) {
    // Update SDL texture with pixel data from a 1D array representing the screen
    // Pitch: The number of bytes per scanline in the screen array
    SDL_UpdateTexture(texture_sdl , nullptr, &SCREEN, info::INTERNAL_SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer_sdl);
    SDL_RenderCopy(renderer_sdl, texture_sdl, nullptr, nullptr);
    SDL_RenderPresent(renderer_sdl);
}

std::shared_ptr<display> display::getInstance() {
    if (instance == nullptr) {
        instance = std::shared_ptr<display>(new display());
    }
    return instance;
}