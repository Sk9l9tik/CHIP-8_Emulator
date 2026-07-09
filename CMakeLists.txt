cmake_minimum_required(VERSION 3.31)
project(CHIP-8_Emulator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(COMPILE_FLAGS -Wall -Wextra -Wpedantic)

find_package(SFML 3 COMPONENTS graphics audio QUIET)
if(SFML_FOUND)
    message(STATUS "Using system SFML: ${SFML_DIR}")
else()
    FetchContent_Declare(sfml
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 3.0.2)
    FetchContent_MakeAvailable(sfml)
endif()

find_package(GTest QUIET)
if(GTest_FOUND)
    message(STATUS "Using system Gtest: ${GTest_DIR}")
else()
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0)
    FetchContent_MakeAvailable(googletest)
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

#target_compile_options(chip8_emu PRIVATE
#    ${COMPILE_FLAGS}
#    $<$<CONFIG:Debug>:-g -O0 -fsanitize=address,undefined>
#    $<$<CONFIG:Release>:-O3>
#)
#
#target_link_options(chip8_emu PRIVATE
#    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
#)

# Rabota? No
# SAERMO? YES do slez
target_compile_options(chip8_emu PRIVATE
        ${COMPILE_FLAGS}
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>,$<NOT:$<PLATFORM_ID:Windows>>>:-g -O0 -fsanitize=address,undefined>
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>,$<NOT:$<PLATFORM_ID:Windows>>>:-g -O0 -fsanitize=address,undefined>
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3>
)

target_link_options(chip8_emu PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>,$<NOT:$<PLATFORM_ID:Windows>>>:-fsanitize=address,undefined>
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>,$<NOT:$<PLATFORM_ID:Windows>>>:-fsanitize=address,undefined>
)

file(GLOB_RECURSE TEST_CORE_SRC tests/core/*.cpp)
file(GLOB_RECURSE TEST_DEBUG_SRC tests/debug/*.cpp)
file(GLOB_RECURSE TEST_UI_SRC tests/ui/*.cpp)

add_executable(chip8_tests
    ${TEST_CORE_SRC}
    ${TEST_DEBUG_SRC}
    ${TEST_UI_SRC}

    ${CORE_SRC}
    ${DEBUG_SRC}
    # ${UI_SRC}
)

target_include_directories(chip8_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(chip8_tests PRIVATE GTest::gtest GTest::gtest_main)

enable_testing()

include(GoogleTest)
gtest_discover_tests(chip8_tests)
