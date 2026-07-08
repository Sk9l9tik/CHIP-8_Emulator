#include <gtest/gtest.h>
#include "core/CPU.h"

TEST(CPU, ResetSetsVRegistersToZeroPCToROMStartSPAndIndexAndSTandDTToZero){
    Memory mem{};
    FrameBuffer fb{};
    CPU cpu{mem, fb};

    mem.write(0x200, 0x60);
    mem.write(0x201, 0x78);

    cpu.tick();

    EXPECT_EQ(cpu.get_register(0), 0x78);

    mem.write(0x202, 0xF0);
    mem.write(0x203, 0x15);

    cpu.tick();

    EXPECT_EQ(cpu.get_delay_timer(), 0x78);

    mem.write(0x204, 0xA3);
    mem.write(0x205, 0xFF);

    cpu.tick();

    EXPECT_EQ(cpu.get_index(), 0x3FF);

    mem.write(0x206, 0xF0);
    mem.write(0x207, 0x18);

    cpu.tick();

    EXPECT_EQ(cpu.get_sound_timer(), 0x78);

    mem.write(0x208, 0x22);
    mem.write(0x209, 0xFF);

    cpu.tick();

    EXPECT_EQ(cpu.get_PC(), 0x2FF);
    EXPECT_EQ(cpu.get_stack_pointer(), 0x1);
    EXPECT_EQ(cpu.get_stack(0), 0x20A);

    cpu.reset();
    for(uint8_t i = 0; i < 16; ++i)
        EXPECT_EQ(cpu.get_register(i), 0x00);

    EXPECT_EQ(cpu.get_PC(), 0x200);
    EXPECT_EQ(cpu.get_stack_pointer(), 0);
    EXPECT_EQ(cpu.get_stack(0), 0);
    EXPECT_EQ(cpu.get_delay_timer(), 0);
    EXPECT_EQ(cpu.get_sound_timer(), 0);
}