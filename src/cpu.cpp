#include <iomanip>
#include "cpu.h"

// At first-execution, indicates that no cpu instance
// has been initialised yet
std::shared_ptr<cpu> cpu::instance = nullptr;


cpu::cpu() {
    draw_flag = false;
    std::copy(font_set.begin(), font_set.end(), memory.begin());    // Copy font_set into beginning of memory
    
    i = 0, delay_timer = 0, opcode = 0, sound_timer = 0, stack_pointer = 0;

    pc = info::ROM_START_ADDRESS;
    SCREEN.fill(info::BG_COLOR);
    memory.fill(0);
    stack.fill(0);
    v.fill(0);

    gen = std::default_random_engine(std::random_device()());

    distribution = std::uniform_int_distribution<unsigned int>(0, 255);

    state = STATE_RUNNING;
    std::cout << "CHIP8 core started." << std::endl;
}

cpu::~cpu() {
    std::cout << "CHIP8 core destroyed." << std::endl;
} 

bool cpu::loadrom(const char* rom_name) {
    // Load ROM
    FILE* rom;
    errno_t err;

    if ((err = fopen_s(&rom, rom_name, "rb") != 0)) {
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
    return true;
}

void cpu::fetch() {
    // Convert the big-endian opcode to little-endian format.
	// This involves shifting the upper byte into the corresponding little endian position
	// and OR-ing it with the lower little-endian byte
    // (memory[pc] = first 2 bytes, memory[pc+1] next 2 bytes)
    opcode = (memory[pc] << 8) | memory[pc + 1];
}

void cpu::execute() {
    printf("Address: 0x%04X, Opcode: 0x%04X \n", pc - 2, opcode);
    //std::cout << "prefix: " << prefix(opcode) << std::endl;
    uint16_t result = 0;
    switch (prefix(opcode)) {
        case 0x0:
            // 0x0EN branch
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
            break;
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
            if (v[x(opcode)] == nn(opcode)) {
                skip();
            }
            break;
        case 0x4:
            // 0x4XNN: Skip next instruction if VX != NN
            if (v[x(opcode)] != nn(opcode)) {
                skip();
            }
            break;
        case 0x5:
            // 0x5XY0: Skip next instruction if VX == VY
            if (v[x(opcode)] == v[y(opcode)]) {
                skip();
            }
            break;
        case 0x6:
            // 0x6XNN: Set VX to NN
            v[x(opcode)] = nn(opcode);
            break;
        case 0x7:
            // 0x7XNN: Add NN to VX
            v[x(opcode)] += nn(opcode);
            break;
        case 0x8:
            // 0x8XYN branch
            switch (n(opcode)) {
                case 0x0:
                    // 0x8XY0: Set VX to VY
                    v[x(opcode)] = v[y(opcode)];
                    break;
                case 0x1:
                    // 0x8XY1: Set VX to VX OR VY
                    v[x(opcode)] |= v[y(opcode)];
                    break;
                case 0x2:
                    // 0x8XY2: Set VX to VX AND VY
                    v[x(opcode)] &= v[y(opcode)];
                    break;
                case 0x3:
                    // 0x8XY3: Set VX to VX XOR VY
                    v[x(opcode)] ^= v[y(opcode)];
                    break;
                case 0x4:
                    // 0x8XY4: Set VX to VX ADD VYf; VF set to 1 if there is a carry, 0 if there isn't
                    result = v[x(opcode)] + v[y(opcode)];
                    if (result > 255) {
                        v[0xF] = 1; // Carry occured, set VF to 1
                    }
                    else {
                        v[0xF] = 0; // No carry, set VF to 0
                    }
                    v[x(opcode)] += v[y(opcode)];
                    break;
                case 0x5:
                    // 0x8XY5: Set VX to VX SUB VY result; VF set to 0 if there is a borrow, 1 when there isn't
                    if (v[x(opcode)] >= v[y(opcode)]) {
                        v[0xF] = 1; // No borrow, set VF to 1
                    } 
                    else {
                        v[0xF] = 0; // Borrow occured, set VF to 0
                    }
                    v[x(opcode)] -= v[y(opcode)];
                    break;
                case 0x6: 
                    // 0x8XY6: Set VF to least significant bit of VX; shift VX to the right by 1
                    v[0xF] = v[x(opcode)] & 0x1;
                    v[x(opcode)] >>= 1;
                    break;
                case 0x7:
                    // 0x8XY7: Set VX to VY SUB VX result; VF set to 0 if there is a borrow; 1 when there isn't
                    if (v[y(opcode)] >= v[x(opcode)]) {
                        v[0xF] = 1; // No borrow, set VF to 1
                    }
                    else {
                        v[0xF] = 0; // Borrow occured, set VF to 0
                    }
                    v[x(opcode)] = v[y(opcode)] - v[x(opcode)];
                    break;
                case 0xE:
                    // 0x8XYE: Set VF to most significant bit of VX; shift VX left by 1
                    v[0xF] = v[x(opcode)] & 0x80;
                    v[x(opcode)] <<= 1;
                    break;
                default:
                    illegal();
                    break;
            }
            break;
        case 0x9:
            // 0x9XY0: Skips next instruction if VX != VY
            if (v[x(opcode)] != v[y(opcode)]) {
                skip();
            }
            break;
        case 0xA:
            // 0xANNN: Set I to NNN
            i = nnn(opcode);
            break;
        case 0xB:
            // 0xBNNN: Jump to address NNN ADD V0
            jump(v[0]);
            break;
        case 0xC: 
            // 0xCXNN: Set VX to random_number(0-255) bitwise AND NN
            v[x(opcode)] = distribution(gen) & nn(opcode);
        case 0xD:
            draw();
            break;
        /*case 0xE:
            // 0xEXNN branch
            switch nn(opcode) {
                case 0x9E: 
                    // 0xEX9E: Skips next instruction if key stored in VX is pressed
            }*/
        default:
            illegal();
            break;

    }
}

void cpu::draw() {
    v[0xF] = 0;
    uint16_t x_root = v[x(opcode)] % info::INTERNAL_SCREEN_WIDTH;
    uint16_t y_root = v[y(opcode)] % info::INTERNAL_SCREEN_HEIGHT;
    for (uint16_t row = 0; row < n(opcode); row++) {
        uint16_t line = memory[i + row];
        for (uint16_t width = 0; width < 8; width++) {
            // Convert 2D X,Y into 1D pixel index for screen array
            //if ((x_root + row) >= info::INTERNAL_SCREEN_WIDTH) break;
            uint16_t pixel_index = (y_root + row) * info::INTERNAL_SCREEN_WIDTH + (x_root + width);   
            if (line & (0x80 >> width)) {
                if (SCREEN[pixel_index] == info::FG_COLOR) {
                    // If current pixel in sprite row is on AND
                    // pixel at X, Y in SCREEN is on
                    v[0xF] = 1;
                }
                SCREEN[pixel_index] ^= info::FG_COLOR;
            }
        }
    }
    draw_flag = true;
}

void cpu::illegal() {
    std::cout << "ERROR: Opcode 0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << opcode <<
        " does not exist." << std::endl;
}

void cpu::cycle() {
    if (pc>info::MEMORY_SIZE){
        std::cerr << "ERROR: Program counter out of bounds. Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }
    draw_flag = false;
    fetch();

    // Increment the program counter to point to the next opcode
    pc+=2;
    execute();
    pc = pc;
}

void cpu::clear_screen() {
    SCREEN.fill(info::BG_COLOR);
}

void cpu::jump(uint8_t addr_offset) {
    // addr_offset overload to offset address for certiain opcodes
    pc = nnn(opcode) + addr_offset;
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