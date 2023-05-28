cmake_minimum_required (VERSION 3.8)

find_package(SDL2 REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)

if (SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/include)
    target_include_directories(CHIP8x PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/include)
    add_library(cpu STATIC cpu.cpp)
    add_library(display STATIC display.cpp)
    target_link_libraries(CHIP8x PRIVATE ${SDL2_LIBRARIES} cpu display SDL2::SDL2 SDL2::SDL2main)
endif ()