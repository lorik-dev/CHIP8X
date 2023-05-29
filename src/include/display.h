#ifndef CHIP8X_DISPLAY_H
#define CHIP8X_DISPLAY_H

#pragma once

#include <iostream>
#include <memory>
#include <array>
#include "data.h"
#include "cpu.h"
#include <SDL.h>

class display {

private:

    display();

    SDL_Window* window_sdl;

    SDL_Renderer* renderer_sdl;

    SDL_Texture* texture_sdl;

    static std::shared_ptr<display> instance;


public:
    ~display();

    bool init_sdl();

    void update_screen(std::array<uint32_t, info::INTERNAL_SCREEN_PIXELS> &SCREEN);

    static std::shared_ptr<display> getInstance();
};

#endif //CHIP8X_DISPLAY_H