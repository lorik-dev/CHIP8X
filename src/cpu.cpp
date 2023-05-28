#include <iomanip>

#include "cpu.h"

// At first-execution, indicates that no cpu instance
// has been initialised yet
std::shared_ptr<cpu> cpu::instance = nullptr;

cpu::cpu() {
    std::copy(font_set.begin(), font_set.end(), memory.begin());    // Copy font_set into beginning of memory
    
    pc = info::ROM_START_ADDRESS;
    display.fill(info::BG_COLOR);
    memory.fill(0);
    stack.fill(0);
    v.fill(0);

    state = STATE_RUNNING;
    std::cout << "CHIP8 core started." << std::endl;
}

cpu::~cpu() {
    std::cout << "CHIP8 core destroyed." << std::endl;
} 

bool cpu::loadrom(const char* rom_name) {
    // Load ROM
    FILE* rom = fopen(rom_name, "rb");
    if (!rom) {
        std::cerr << "ERROR: ROM file " << rom_name << " could not be opened." << std::endl;
        return false;
    }

    // Read the ROM data into RAM
    fseek(rom, 0L, SEEK_END); // Seek end of the rom
    long rom_size = ftell(rom); // Get current position, which is the ROM size in bytes
    if (rom_size > info::ROM_MAX_SIZE) {
        std::cerr << "ERROR: ROM file " << rom_name << " is too large (" << rom_size << " bytes); max size allowed: " << info::ROM_MAX_SIZE << " bytes" << std::endl;
        return false;
    }
    rewind(rom);

    if (fread(&memory[pc], rom_size, 1, rom) != 1) {
        std::cerr << "ERROR: ROM file " << rom_name << " could not be read into CHIP8 memory" << std::endl;
        return false;
    }

    fclose(rom);
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
        case 0x1:
            // 0x1NNN: Jump to NNNN
            jump();
            break;
        case 0x2:
            // 0x2NNN: Call subroutine at NNN
            call();
            break;
        case 0x3:
            // 0x3XNN: Skip next instruction if VX == NN
            if (stack[x(opcode)] == nn(opcode)) {
                skip();
                break;
            }
        case 0x4:
            // 0x4XNN: Skip next instruction if VX != NN
            if (stack[x(opcode)] != nn(opcode)) {
                skip();
                break;
            }
        case 0x5:
            // 0x5XY0: Skip next instruction if VX == VY
            if (stack[x(opcode)] == stack[y(opcode)]) {
                skip();
                break;
            }
        case 0x6:
            // 0x6XNN: Set VX to NN
            stack[x(opcode)] = nn(opcode);
            break;
        case 0x7:
            // 0x7XNN: Add NN to VX
            stack[x(opcode)] += nn(opcode);
            break;





    }
}

void cpu::illegal() {
    std::cout << "ERROR: Opcode 0x" << std::setfill('0') << std::setw(4) << std::hex << opcode << 
        " does not exist. Exiting..." << std::endl;
}

void cpu::cycle() {
    if (pc>info::MEMORY_SIZE){
        std::cerr << "ERROR: Program counter out of bounds. Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }
    fetch();

    // Increment the program counter to point to the next opcode
    pc+=2;
}

void cpu::clear_screen() {
    display.fill(info::BG_COLOR);
}

void cpu::jump() {
    pc = nnn(opcode);
}

void cpu::call() {
    if (stack_pointer < stack.size()) {
        stack[stack_pointer] = pc;
        stack_pointer++;
    }
    else {
        std::cerr << "ERROR: Stack overflow. Stack size: " << stack.size() << ". Exiting ..." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void cpu::skip() {
    // PC is already incremented after fetch()
    // so to skip next instruction, PC must be incremented to next opcode
    pc += 2;
}

void cpu::subret() {
    stack_pointer--;
    pc = stack[stack_pointer];
}

std::shared_ptr<cpu> cpu::getInstance() {
    if (instance == nullptr) {
        instance = std::shared_ptr<cpu>(new cpu());
    }
    return instance;
}