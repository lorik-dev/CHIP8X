#ifndef CHIP8X_DISPLAY_H
#define CHIP8X_DISPLAY_H

#pragma once

#include <iostream>
#include <memory>
#include <array>
#include "data.h"

class display {

private:
    display();

    static std::shared_ptr<display> instance;

public:
    ~display();
};

#endif //CHIP8X_DISPLAY_H