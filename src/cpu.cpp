#include <iostream>
#include <iomanip>

#include "include/cpu.h"
#include "include/data.h"

std::shared_ptr<cpu> cpu::instance = nullptr;

cpu::cpu() {
    std::copy(font_set.begin(), font_set.end(), memory.begin());    // Copy font_set into beginning of memory
    
    display.fill(info::BG_COLOR);
    memory.fill(0);
    stack.fill(0);
    v.fill(0); 
}

void cpu::fetch() {
    // Convert the big-endian opcode to little-endian format.
	// This involves shifting the upper byte into the corresponding little endian position
	// and OR-ing it with the lower little-endian byte
    // (memory[pc] = first 2 bytes, memory[pc+1] next 2 bytes)
    opcode = (memory[pc] << 8) | memory[pc + 1];
}

void cpu::execute() {
    switch (prefix(opcode)) {
        case 0x0:
            switch(nn(opcode)){
                case 0xE0:
                    // 0x00E0: Clear the screen
                    clear_screen();
                    break;
                case 0xEE:
                    // 0x00EE: Return from a subroutine
                    subret();
                    break;
                default:
                    illegal();
                    break;
            } 
        //case 0x1:
        case 0x2:
            

    }
}

void cpu::illegal() {
    std::cout << "ERROR: Opcode 0x" << std::setfill('0') << std::setw(4) << std::hex << opcode << " does not exist. Exiting..." << std::endl;
}

void cpu::cycle() {
    if (pc>info::MEMORY_SIZE){
        std::cerr << "ERROR: Program counter out of bounds. Exiting..." << std::endl;
        exit(-1);
    }
    fetch();

    // Increment the program counter to point to the next opcode
    pc+=2;
}

void cpu::clear_screen() {
    display.fill(info::BG_COLOR);
}

void cpu::subret() {
    stack_pointer--;
    pc = stack[stack_pointer];
}