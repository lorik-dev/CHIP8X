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
    SDL_Window* window_sdl;
    SDL_Renderer* renderer_sdl; 

    display();

    static std::shared_ptr<display> instance;


public:
    ~display();

    bool init_sdl();

    static std::shared_ptr<display> getInstance();
};

#endif //CHIP8X_DISPLAY_H