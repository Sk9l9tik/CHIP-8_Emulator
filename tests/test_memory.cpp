#include <gtest/gtest.h>
#include "core/Memory.h"

TEST(Memory, FontLoadedAtCorrectAddress){
    Memory mem{};

    EXPECT_EQ(mem.read(0x050), 0xF0);
    EXPECT_EQ(mem.read(0x051), 0x90);
    EXPECT_EQ(mem.read(0x054), 0xF0);
}