cmake_minimum_required(VERSION 3.31)
project(CHIP-8_Emulator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chip8_emu
        src/main.cpp
        src/CPU.cpp)

target_include_directories(chip8_emu PRIVATE ${PROJECT_SOURCE_DIR}/include)