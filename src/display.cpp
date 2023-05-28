#include <iostream>
#include <iomanip>

#include "include/display.h"
#include "include/data.h"
#include <SDL.h>

std::shared_ptr<display> display::instance = nullptr;

display::display() {

    std::cout << "Display started." << std::endl;
}

display::~display() {
    std::cout << "Display destroyed." << std::endl;
}