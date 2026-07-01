cmake_minimum_required(VERSION 3.31)
project(CHIP-8_Emulator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

find_package(SFML 3 COMPONENTS graphics audio QUIET)
if(SFML_FOUND)
    message(STATUS "Using system SFML: ${SFML_DIR}")
else()
    FetchContent_Declare(sfml
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 3.0.2)
    FetchContent_MakeAvailable(sfml)
endif()

file(GLOB_RECURSE CORE_SRC src/core/*.cpp)
file(GLOB_RECURSE DEBUG_SRC src/debug/*.cpp)
file(GLOB_RECURSE UI_SRC src/ui/*.cpp)

add_executable(chip8_emu
    ${CORE_SRC}
    ${DEBUG_SRC}
    ${UI_SRC}
    src/main.cpp
)

target_include_directories(chip8_emu PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(chip8_emu PRIVATE sfml-graphics sfml-audio)
