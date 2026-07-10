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

//чёт поздновато я додумался до такого прикола

class CPU_Test : public ::testing::Test{
protected:
    Memory mem{};
    FrameBuffer fb{};
    CPU cpu{mem, fb};

    void SetUp() override{ //конченные camel-case придётся весь в этом стиле писать
        cpu.reset();
    }

    [[deprecated("Use loadOpcodeAndExecute")]] void loadOpcode(uint16_t opcode, uint16_t address = 0x200){
        mem.write(address, (opcode >> 8) & 0xFF);
        mem.write(address + 1, opcode & 0xFF);
    }

    void loadOpcodeAndExecute(uint16_t opcode, uint16_t address = 0x200){
        mem.write(address, (opcode >> 8) & 0xFF);
        mem.write(address + 1, opcode & 0xFF);
        cpu.tick();
    }

    void executeOpcode(uint16_t opcode){
        loadOpcode(opcode);
        cpu.reset();
        cpu.tick();
    }
};

TEST_F(CPU_Test, FrameBufferIsClearedAfterCLS){
    uint8_t sprite_data[] {0xFF, 0xFF};
    static_cast<void>(fb.draw_sprite(0, 0, sprite_data, 1));

    EXPECT_TRUE(fb.get_frame_buffer().at(0));

    executeOpcode(0x00E0);
    EXPECT_FALSE(fb.get_frame_buffer().at(0));
}

TEST_F(CPU_Test,CALLSavesPCIncreasesSPSetsPCToNNN_RETSetsPCtoStackTopAndDecreasesSP){
    executeOpcode(0x23FF);
    EXPECT_EQ(cpu.get_PC(), 0x03ff);
    uint16_t return_pos {cpu.get_stack(cpu.get_stack_pointer()-1)};
    EXPECT_EQ(cpu.get_stack_pointer(), 1);

    loadOpcode(0x00EE, 0x03ff);
    cpu.tick();
    EXPECT_EQ(cpu.get_PC(), return_pos);
    EXPECT_EQ(cpu.get_stack_pointer(), 0);
}

TEST_F(CPU_Test, JMPSetsPCToNNN){
    executeOpcode(0x1233);
    EXPECT_EQ(cpu.get_PC(), 0x233);
}

TEST_F(CPU_Test, LDVxSetsVx_SESkipsInstructionIfVxEqualsNNElseDoesntSkip){
    loadOpcode(0x6078, 0x200);
    cpu.tick();

    EXPECT_EQ(cpu.get_register(0), 0x78);

    loadOpcode(0x3078, 0x202);
    cpu.tick();
    EXPECT_EQ(cpu.get_PC(), 0x206);

    loadOpcode(0x3079,0x206);
    cpu.tick();
    EXPECT_EQ(cpu.get_PC(), 0x208);
}

TEST_F(CPU_Test, SNESkipsInstructionIfVxDoesntEqualNNElseDoesntSkip){
    loadOpcode(0x6078, 0x200);
    cpu.tick();

    loadOpcode(0x4078, 0x202);
    cpu.tick();
    EXPECT_EQ(cpu.get_PC(), 0x204);

    loadOpcode(0x4079, 0x204);
    cpu.tick();
    EXPECT_EQ(cpu.get_PC(), 0x208);
}

TEST_F(CPU_Test, SESkipsInstructionIfVxEqualsVyElseDoesntSkip){
    loadOpcode(0x6078, 0x200);
    loadOpcode(0x6178, 0x202);
    loadOpcode(0x6279, 0x204);

    cpu.tick();
    cpu.tick();
    cpu.tick();

    loadOpcode(0x5010, 0x206);
    cpu.tick();

    EXPECT_EQ(cpu.get_PC(), 0x20A);

    loadOpcode(0x5020, 0x20A);
    cpu.tick();
    EXPECT_EQ(cpu.get_PC(), 0x20C);
}

TEST_F(CPU_Test, ADDIncreasesVxByNNAndSetsVfOnCarry){
    loadOpcode(0x6078, 0x200);
    cpu.tick();

    loadOpcode(0x7078, 0x202);
    cpu.tick();

    EXPECT_EQ(cpu.get_register(0), 0xF0);
    loadOpcode(0x7010, 0x204);
    cpu.tick();

    EXPECT_EQ(cpu.get_register(0), 0x00);
    EXPECT_NE(cpu.get_register(15), 0x1);
}

TEST_F(CPU_Test, LDVxVySetsVxToVy){
    loadOpcode(0x6178, 0x200);
    cpu.tick();

    loadOpcode(0x8010, 0x202);
    cpu.tick();

    EXPECT_EQ(cpu.get_register(0), cpu.get_register(1));
}

TEST_F(CPU_Test, ORVxVySetsVxToVxORVy){
    loadOpcodeAndExecute(0x6101, 0x200);

    loadOpcodeAndExecute(0x6080, 0x202);

    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8011, 0x204);

    EXPECT_EQ(cpu.get_register(0), Vx | cpu.get_register(1));
}

TEST_F(CPU_Test, ANDVxVySetsVxToVxANDVy){
    loadOpcodeAndExecute(0x601F, 0x200);
    loadOpcodeAndExecute(0x61F0, 0x202);

    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8012, 0x204);
    EXPECT_EQ(cpu.get_register(0), Vx & cpu.get_register(1));
}

TEST_F(CPU_Test, XORVxVySetsVxToVxXORVy){
    loadOpcodeAndExecute(0x60F0, 0x200);
    loadOpcodeAndExecute(0x610F, 0x202);

    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8013, 0x204);

    EXPECT_EQ(cpu.get_register(0), Vx ^ cpu.get_register(1));
}

TEST_F(CPU_Test, ADDVxVyWithoutCarryJustIncreasesVxByVy_WithCarryAlsoSetsVfIfVxGetsAboveFF){
    loadOpcodeAndExecute(0x6078, 0x200);
    loadOpcodeAndExecute(0x6178, 0x202);

    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8014, 0x204);

    EXPECT_EQ(cpu.get_register(0), Vx + cpu.get_register(1));

    Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8014, 0x206);

    EXPECT_EQ(cpu.get_register(0), static_cast<uint8_t>(Vx + cpu.get_register(1)));
    EXPECT_EQ(cpu.get_register(15), 1);
}

TEST_F(CPU_Test, SUBVxVyDecresesVxByVyAndSetsVfIfVxIsGreaterThanVy){
    loadOpcodeAndExecute(0x6078,0x200);
    loadOpcodeAndExecute(0x6177, 0x202);

    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8015, 0x204);

    EXPECT_EQ(cpu.get_register(0), Vx - cpu.get_register(1));
    EXPECT_EQ(cpu.get_register(15), 1);

    Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8015, 0x206);

    EXPECT_EQ(cpu.get_register(0), static_cast<uint8_t>(Vx - cpu.get_register(1)));
    EXPECT_EQ(cpu.get_register(15), 0);
}

TEST_F(CPU_Test, SHRBitShiftsVxToRight_SetsVfIfLeastSignificantBitIsSet){
    loadOpcodeAndExecute(0x6002, 0x200);

    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8016, 0x202);

    EXPECT_EQ(cpu.get_register(0), Vx >> 1);
    EXPECT_EQ(cpu.get_register(15), 0);

    Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8016, 0x204);

    EXPECT_EQ(cpu.get_register(0), static_cast<uint8_t>(Vx >> 1));
    EXPECT_EQ(cpu.get_register(15), 1);
}

TEST_F(CPU_Test, SUBVxVySetsVxToDifferenceBetweenVyAndVx_SetsVfIfVyIsGreaterThanVx){
    loadOpcodeAndExecute(0x6001, 0x200);
    loadOpcodeAndExecute(0x61FF, 0x202);

    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x8017, 0x204);

    EXPECT_EQ(cpu.get_register(0), static_cast<uint8_t>(cpu.get_register(1) - Vx));
    EXPECT_EQ(cpu.get_register(15), 1);

    Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x6101, 0x206);
    loadOpcodeAndExecute(0x8017, 0x208);

    EXPECT_EQ(cpu.get_register(0), static_cast<uint8_t>(cpu.get_register(1) - Vx));
    EXPECT_EQ(cpu.get_register(15), 0);

}

TEST_F(CPU_Test, SHLBitShiftsVxToLeft_SetsVfIfMostSignificantBitIsSet){
    loadOpcodeAndExecute(0x6040, 0x200);

    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x801E, 0x202);

    EXPECT_EQ(cpu.get_register(0), Vx << 1);
    EXPECT_EQ(cpu.get_register(15), 0);

    Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0x801E, 0x204);

    EXPECT_EQ(cpu.get_register(0), static_cast<uint8_t>(Vx << 1));
    EXPECT_EQ(cpu.get_register(15), 1);
}

TEST_F(CPU_Test, SNESkipsInstructionIfVxDoesntEqualVyElseDoesntSkip){
    loadOpcodeAndExecute(0x6078, 0x200);
    loadOpcodeAndExecute(0x6178, 0x202);
    loadOpcodeAndExecute(0x9010, 0x204);

    EXPECT_EQ(cpu.get_PC(), 0x206);

    loadOpcodeAndExecute(0x6079, 0x206);
    loadOpcodeAndExecute(0x9010, 0x208);
    EXPECT_EQ(cpu.get_PC(), 0x20C);
}

TEST_F(CPU_Test, LDSetsIToNNN){
    loadOpcodeAndExecute(0xA333, 0x200);
    EXPECT_EQ(cpu.get_index(), 0x333);
}

TEST_F(CPU_Test, RNDGeneratesAValueInUint8Range){
    loadOpcodeAndExecute(0xC0FF, 0x200);

    EXPECT_TRUE(std::in_range<uint8_t>(cpu.get_register(0)));
}

TEST_F(CPU_Test, JMPSetsPCToVxPlusNNN){
    loadOpcodeAndExecute(0x6078, 0x200);
    uint8_t Vx = cpu.get_register(0);

    loadOpcodeAndExecute(0xB333, 0x202);

    EXPECT_EQ(cpu.get_PC(), 0x333 + Vx);
}

TEST_F(CPU_Test, DRWSetsVfIfCollisionAndDrawsAtCorrectPos){
    loadOpcodeAndExecute(0x6078);
    loadOpcodeAndExecute(0x6175, 0x202);
    loadOpcodeAndExecute(0xA050, 0x204);
    loadOpcodeAndExecute(0xD017, 0x206);

    EXPECT_FALSE(cpu.get_register(15));

    const auto& pixels {fb.get_frame_buffer()};

    uint8_t px = (0x78) % 64;
    uint8_t py = (0x75) % 32;

    for(uint8_t i = 0; i < 8; ++i){
        if(i < 4) EXPECT_TRUE(pixels.at(py*64+px+i));
        else EXPECT_FALSE(pixels.at(py*64+px+i));
    }

    loadOpcodeAndExecute(0xD017, 0x208);

    EXPECT_TRUE(cpu.get_register(15));

    for(uint8_t i = 0; i < 8; ++i)
        EXPECT_FALSE(pixels.at(py*64+px+i));

}

TEST_F(CPU_Test, SKPSkipsInstructionIfButtonNumberVxIsPressed){
    cpu.set_key_state(0, true);
    loadOpcodeAndExecute(0xE09E, 0x200);

    EXPECT_EQ(cpu.get_PC(), 0x204);

    loadOpcodeAndExecute(0x6101, 0x204);

    loadOpcodeAndExecute(0xE19E, 0x206);
    EXPECT_EQ(cpu.get_PC(), 0x208);
}

TEST_F(CPU_Test, SNKPSkipsInstructionIfButtonNumberVxIsNotPressed){
    cpu.set_key_state(0, true);
    loadOpcodeAndExecute(0xE0A1, 0x200);

    EXPECT_EQ(cpu.get_PC(), 0x202);

    loadOpcodeAndExecute(0x6101, 0x202);

    loadOpcodeAndExecute(0xE1A1, 0x204);
    EXPECT_EQ(cpu.get_PC(), 0x208);
}

TEST_F(CPU_Test, LDSetsVxToDT_LDSetsDTToVx_LDSetsSTToVx){
    loadOpcodeAndExecute(0x6078, 0x200);

    loadOpcodeAndExecute(0xF015, 0x202);

    EXPECT_EQ(cpu.get_register(0), cpu.get_delay_timer());

    loadOpcodeAndExecute(0xF018, 0x204);

    EXPECT_EQ(cpu.get_register(0), cpu.get_sound_timer());

    loadOpcodeAndExecute(0x6077, 0x206);

    EXPECT_NE(cpu.get_register(0), cpu.get_delay_timer());

    loadOpcodeAndExecute(0xF007, 0x208);

    EXPECT_EQ(cpu.get_register(0), cpu.get_delay_timer());
}

TEST_F(CPU_Test, LDWaitsForInputAndSetsKeyVxToPressed){
    loadOpcodeAndExecute(0x6001, 0x200);

    loadOpcodeAndExecute(0xF00A, 0x202);

    cpu.tick();

    EXPECT_EQ(cpu.get_PC(), 0x204);

    cpu.set_key_state(0x1, true);

    loadOpcodeAndExecute(0x6078, 0x204);

    EXPECT_EQ(cpu.get_PC(), 0x206);
}

TEST_F(CPU_Test, ADDSetsIToIPlusVx){
    loadOpcodeAndExecute(0x6078, 0x200);

    loadOpcodeAndExecute(0xA333, 0x202);

    loadOpcodeAndExecute(0xF01E, 0x204);

    EXPECT_EQ(cpu.get_index(), 0x078 + 0x333);
}

TEST_F(CPU_Test, LDWritesBinaryDec){
    loadOpcodeAndExecute(0x60FE, 0x200);

    loadOpcodeAndExecute(0xA333, 0x202);

    loadOpcodeAndExecute(0xF033, 0x204);

    EXPECT_EQ(mem.read(cpu.get_index()), 2);
    EXPECT_EQ(mem.read(cpu.get_index() + 1), 5);
    EXPECT_EQ(mem.read(cpu.get_index() + 2), 4);
}

TEST_F(CPU_Test, LDWritesRegistersFromV0ToVxIntoMemory_LDReadsItFromMemoryIntoRegisters){
    loadOpcodeAndExecute(0x60FE, 0x200);

    loadOpcodeAndExecute(0xA333, 0x202);

    loadOpcodeAndExecute(0x6178, 0x204);
    loadOpcodeAndExecute(0x6213, 0x206);

    loadOpcodeAndExecute(0xF255, 0x208);

    for(uint8_t i = 0; i < 3; ++i)
        EXPECT_EQ(mem.read(cpu.get_index() + i), cpu.get_register(i));

    loadOpcodeAndExecute(0x6000, 0x20A);
    loadOpcodeAndExecute(0x6100, 0x20C);
    loadOpcodeAndExecute(0x6222, 0x20E);

    loadOpcodeAndExecute(0xF265, 0x210);

    for(uint8_t i = 0; i < 3; ++i)
        EXPECT_EQ(mem.read(cpu.get_index() + i), cpu.get_register(i));
}