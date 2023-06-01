#include <iostream>
#include "display.h"
#include <chrono>


int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: ./CHIP8x rom_file [scale_factor]\n");
		exit(EXIT_FAILURE);
	}
	if (argc == 3) {
		uint32_t result = std::strtoul(argv[2], nullptr, 0);
		if (result != 0) {
			info::SCALE_FACTOR = result;
			std::cout << "INFO: Scale factor set to " << result << " (" << info::INTERNAL_SCREEN_WIDTH*result << "*" << info::INTERNAL_SCREEN_HEIGHT*result << ") " << std::endl;
		}
		else {
			info::SCALE_FACTOR = info::DEFAULT_SCALE_FACTOR;
			std::cout << "INFO: Scale factor set to " << info::DEFAULT_SCALE_FACTOR << " (" << info::INTERNAL_SCREEN_WIDTH * info::DEFAULT_SCALE_FACTOR << "*" << info::INTERNAL_SCREEN_HEIGHT * info::DEFAULT_SCALE_FACTOR << ") " << std::endl;
		}
	}
	else {
		info::SCALE_FACTOR = info::DEFAULT_SCALE_FACTOR;
		std::cout << "INFO: Scale factor set to " << info::DEFAULT_SCALE_FACTOR << " (" << info::INTERNAL_SCREEN_WIDTH * info::DEFAULT_SCALE_FACTOR << "*" << info::INTERNAL_SCREEN_HEIGHT * info::DEFAULT_SCALE_FACTOR << ") " << std::endl;
	}

	const char* rom_name = argv[1];
	std::shared_ptr<cpu> cpu = cpu::getInstance();
	if (cpu->loadrom(argv[1]) == false) {
		exit(EXIT_FAILURE);
	}

	std::shared_ptr<display> display = display::getInstance();

	while (cpu->state != STATE_QUIT) {

		if (cpu->state == STATE_PAUSE) continue;

		auto start = std::chrono::high_resolution_clock::now();

		cpu->cycle();

		auto end = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		double delayTime = info::INSTRUCTION_TIME - (duration / 1000.0);

		if (delayTime > 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(delayTime)));
		}

		display->update_screen(cpu->SCREEN);

	}

	return 0;
}
