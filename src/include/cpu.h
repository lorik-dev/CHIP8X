#ifndef CHIP8X_CPU_H
#define CHIP8X_CPU_H

#pragma once

#include <iostream>
#include <memory>
#include <array>
#include "data.h"

class cpu {

private:
    std::array<uint8_t, info::MEMORY_SIZE> memory{};    // CHIP8 memory; default 4KB

    uint16_t opcode;
    std::array<uint16_t, 16> stack{};   // Subroutine stack
    uint16_t stack_pointer;             // Subroutine stack pointer
    std::array<uint8_t, 16> v{};        // V0-VF data registerss
    uint16_t pc;                        // PC program counter
    uint16_t i;                         // I memory register
    uint8_t delay_timer;                // Decrements at 60hz when >0
    uint8_t sound_timer;                // Decrements at 60hz when >0; beeps when >0

    cpu();

    static std::shared_ptr<cpu> instance;


public:
    ~cpu();

    std::array<uint32_t, info::INTERNAL_SCREEN_PIXELS> display{};	

    void fetch();

    void cycle();

    void illegal();

    void execute();

    void clear_screen();

    void jump();

    void skip();

    void call();

    void subret();
};

#endif //CHIP8X_CPU_H