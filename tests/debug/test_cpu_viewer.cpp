#include <gtest/gtest.h>

#include "core/CPU.h"
#include "core/FrameBuffer.h"
#include "core/Memory.h"

#include "debug/CPUViewer.h"

class CPUViewerTest : public ::testing::Test {
protected:
    Memory mem{};
    FrameBuffer fb{};
    CPU cpu{mem, fb};

    void SetUp() override {
        cpu.reset();
    }

    void loadOpcodeAndExecute(uint16_t opcode, uint16_t address = 0x200) {
        mem.write(address, (opcode >> 8) & 0xFF);
        mem.write(address + 1, opcode & 0xFF);
        cpu.tick();
    }
};

TEST_F(CPUViewerTest, SnapshotOnFreshResetCPUIsAllZeroExceptPC) {
    CPUState state = CPUViewer::snapshot(cpu);

    for (uint8_t i = 0; i < 16; ++i)
        EXPECT_EQ(state.V[i], 0) << "V[" << static_cast<int>(i) << "] should be 0 after reset";

    EXPECT_EQ(state.PC, 0x200);
    EXPECT_EQ(state.I, 0);
    EXPECT_EQ(state.SP, 0);
    EXPECT_EQ(state.DT, 0);
    EXPECT_EQ(state.ST, 0);

    for (uint8_t i = 0; i < 16; ++i)
        EXPECT_EQ(state.stack[i], 0);
}

TEST_F(CPUViewerTest, SnapshotCapturesVRegistersAfterLD) {
    loadOpcodeAndExecute(0x60AB, 0x200); // V0 = 0xAB
    loadOpcodeAndExecute(0x61CD, 0x202); // V1 = 0xCD

    CPUState state = CPUViewer::snapshot(cpu);

    EXPECT_EQ(state.V[0], 0xAB);
    EXPECT_EQ(state.V[1], 0xCD);
    for (uint8_t i = 2; i < 16; ++i)
        EXPECT_EQ(state.V[i], 0);
}

TEST_F(CPUViewerTest, SnapshotCapturesPCAfterJump) {
    loadOpcodeAndExecute(0x1300, 0x200); // JP 0x300

    CPUState state = CPUViewer::snapshot(cpu);
    EXPECT_EQ(state.PC, 0x300);
}

TEST_F(CPUViewerTest, SnapshotCapturesIndexRegister) {
    loadOpcodeAndExecute(0xA3FF, 0x200); // LD I, 0x3FF

    CPUState state = CPUViewer::snapshot(cpu);
    EXPECT_EQ(state.I, 0x3FF);
}

TEST_F(CPUViewerTest, SnapshotCapturesStackAndSPAfterCall) {
    loadOpcodeAndExecute(0x2300, 0x200); // CALL 0x300 -> pushes 0x202

    CPUState state = CPUViewer::snapshot(cpu);
    EXPECT_EQ(state.SP, 1);
    EXPECT_EQ(state.stack[0], 0x202);
    EXPECT_EQ(state.PC, 0x300);
}

TEST_F(CPUViewerTest, SnapshotCapturesMultipleStackFrames) {
    loadOpcodeAndExecute(0x2300, 0x200); // CALL 0x300 -> stack[0] = 0x202, SP = 1
    loadOpcodeAndExecute(0x2400, 0x300); // CALL 0x400 -> stack[1] = 0x302, SP = 2

    CPUState state = CPUViewer::snapshot(cpu);
    EXPECT_EQ(state.SP, 2);
    EXPECT_EQ(state.stack[0], 0x202);
    EXPECT_EQ(state.stack[1], 0x302);
    EXPECT_EQ(state.PC, 0x400);
}

TEST_F(CPUViewerTest, SnapshotCapturesDelayTimer) {
    loadOpcodeAndExecute(0x6078, 0x200); // V0 = 0x78
    loadOpcodeAndExecute(0xF015, 0x202); // DT = V0

    CPUState state = CPUViewer::snapshot(cpu);
    EXPECT_EQ(state.DT, 0x78);
}

TEST_F(CPUViewerTest, SnapshotCapturesSoundTimer) {
    loadOpcodeAndExecute(0x6078, 0x200); // V0 = 0x78
    loadOpcodeAndExecute(0xF018, 0x202); // ST = V0

    CPUState state = CPUViewer::snapshot(cpu);
    EXPECT_EQ(state.ST, 0x78);
}

TEST_F(CPUViewerTest, SnapshotDoesNotReflectLiveKeypadState) {
    cpu.set_key_state(0x5, true);

    CPUState state = CPUViewer::snapshot(cpu);

    EXPECT_EQ(state.keypad[0x5], 1);
}

TEST_F(CPUViewerTest, SnapshotIsIndependentCopyNotAffectedByFurtherExecution) {
    loadOpcodeAndExecute(0x60AB, 0x200); // V0 = 0xAB
    CPUState state = CPUViewer::snapshot(cpu);

    loadOpcodeAndExecute(0x60FF, 0x202); // V0 = 0xFF afterwards

    EXPECT_EQ(state.V[0], 0xAB);          // snapshot must not change retroactively
    EXPECT_EQ(cpu.get_register(0), 0xFF); // live CPU state did change
}
