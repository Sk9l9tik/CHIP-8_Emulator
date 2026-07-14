#include <cstdint>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "core/CHIP_8.h"

#include "debug/Debugger.h"

namespace {

std::string ibm_logo_rom_path() {
    return std::string("../../assets/roms/IBM_Logo.ch8"); // hardcode )
}

std::vector<uint8_t> ibm_logo_first_bytes() {
    return {0x00, 0xe0, 0xa2, 0x2a, 0x60, 0x0c};
}

} // namespace


class DebuggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        emulator.load_ROM(ibm_logo_rom_path());
    }

    CHIP_8 emulator;
};

// ---------- pause / resume / is_paused ----------

TEST_F(DebuggerTest, StartsUnpaused) {
    Debugger dbg(emulator);
    EXPECT_FALSE(dbg.is_paused());
}

TEST_F(DebuggerTest, PauseSetsPausedState) {
    Debugger dbg(emulator);
    dbg.pause();
    EXPECT_TRUE(dbg.is_paused());
}

TEST_F(DebuggerTest, ResumeClearsPausedState) {
    Debugger dbg(emulator);
    dbg.pause();
    dbg.resume();
    EXPECT_FALSE(dbg.is_paused());
}

TEST_F(DebuggerTest, PauseIsIdempotent) {
    Debugger dbg(emulator);
    dbg.pause();
    dbg.pause();
    EXPECT_TRUE(dbg.is_paused());
}

// ---------- step() ----------

TEST_F(DebuggerTest, StepAdvancesEmulatorByOneTick) {
    ASSERT_EQ(emulator.get_cpu().get_PC(), 0x200);
    Debugger dbg(emulator);
    dbg.step();
    EXPECT_EQ(emulator.get_cpu().get_PC(), 0x202);
}

TEST_F(DebuggerTest, StepIgnoresPausedState) {
    // step() unconditionally calls emulator.tick() regardless of `paused`
    Debugger dbg(emulator);
    dbg.pause();
    dbg.step();
    EXPECT_EQ(emulator.get_cpu().get_PC(), 0x202);
    EXPECT_TRUE(dbg.is_paused()); // step() dont change paused flag
}

// ---------- update() core logic ----------

TEST_F(DebuggerTest, UpdateTicksWhenUnpausedAndNoBreakpointHit) {
    Debugger dbg(emulator);
    dbg.update();
    EXPECT_EQ(emulator.get_cpu().get_PC(), 0x202);
    EXPECT_FALSE(dbg.is_paused());
}

TEST_F(DebuggerTest, UpdateDoesNothingWhenAlreadyPaused) {
    Debugger dbg(emulator);
    dbg.pause();
    dbg.update();
    EXPECT_EQ(emulator.get_cpu().get_PC(), 0x200);
    EXPECT_TRUE(dbg.is_paused());
}

TEST_F(DebuggerTest, UpdatePausesWhenCurrentPCIsBreakpoint) {
    Debugger dbg(emulator);
    dbg.set_breakpoint(0x202);

    dbg.update();
    ASSERT_FALSE(dbg.is_paused());
    ASSERT_EQ(emulator.get_cpu().get_PC(), 0x202);

    dbg.update();
    EXPECT_TRUE(dbg.is_paused());
    EXPECT_EQ(emulator.get_cpu().get_PC(), 0x202); 
    EXPECT_EQ(emulator.get_cpu().get_index(), 0x0000);
}

TEST_F(DebuggerTest, ResumingAtABreakpointImmediatelyRePauses) {
    Debugger dbg(emulator);
    dbg.set_breakpoint(0x202);
    dbg.update(); // -> PC 0x202, still unpaused
    dbg.update(); // -> hits breakpoint, pauses
    ASSERT_TRUE(dbg.is_paused());

    dbg.resume();
    dbg.update();
    EXPECT_TRUE(dbg.is_paused());                  
    EXPECT_EQ(emulator.get_cpu().get_PC(), 0x202);
}

TEST_F(DebuggerTest, UpdateStepsThroughFullDrawLoopThenStopsAtJumpBreakpoint) {
    Debugger dbg(emulator);
    dbg.set_breakpoint(0x228);

    for (int i = 0; i < 20; ++i) {
        dbg.update();
        ASSERT_FALSE(dbg.is_paused()) << "paused early at iteration " << i;
    }
    ASSERT_EQ(emulator.get_cpu().get_PC(), 0x228);

    dbg.update(); 
    EXPECT_TRUE(dbg.is_paused());
    EXPECT_EQ(emulator.get_cpu().get_PC(), 0x228);
}

// ---------- breakpoint delegation to BreakpointManager ----------

TEST_F(DebuggerTest, SetBreakpointDelegatesCorrectly) {
    Debugger dbg(emulator);
    dbg.set_breakpoint(0x250);
    EXPECT_TRUE(dbg.has_breakpoint(0x250));
    EXPECT_EQ(dbg.get_breakpoints().all().size(), 1u);
}

TEST_F(DebuggerTest, RemoveBreakpointDelegatesCorrectly) {
    Debugger dbg(emulator);
    dbg.set_breakpoint(0x250);
    dbg.remove_breakpoint(0x250);
    EXPECT_FALSE(dbg.has_breakpoint(0x250));
}

TEST_F(DebuggerTest, ToggleBreakpointDelegatesCorrectly) {
    Debugger dbg(emulator);
    dbg.toggle_breakpoint(0x260);
    EXPECT_TRUE(dbg.has_breakpoint(0x260));
    dbg.toggle_breakpoint(0x260);
    EXPECT_FALSE(dbg.has_breakpoint(0x260));
}

TEST_F(DebuggerTest, ClearBreakpointsDelegatesCorrectly) {
    Debugger dbg(emulator);
    dbg.set_breakpoint(0x250);
    dbg.set_breakpoint(0x260);
    dbg.clear_breakpoints();
    EXPECT_TRUE(dbg.get_breakpoints().all().empty());
}

// ---------- memory delegation ----------

TEST_F(DebuggerTest, MemoryRangeMatchesRawRomContents) {
    Debugger dbg(emulator);
    const auto expected = ibm_logo_first_bytes();
    auto range = dbg.memory_range(Memory::ROM_START, static_cast<uint16_t>(expected.size()));

    ASSERT_EQ(range.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(range[i], expected[i]) << "mismatch at offset " << i;
        EXPECT_EQ(range[i], emulator.get_memory().read(static_cast<uint16_t>(Memory::ROM_START + i)));
    }
}

TEST_F(DebuggerTest, MemoryDumpProducesNonEmptyOutput) {
    Debugger dbg(emulator);
    std::string dump = dbg.memory_dump(Memory::ROM_START, 16);
    EXPECT_FALSE(dump.empty());
}

// ---------- disassembly / CPU state delegation ----------
// NOTE: kinda strange tests 

TEST_F(DebuggerTest, DisassembleReturnsRequestedInstructionCount) {
    Debugger dbg(emulator);
    auto instructions = dbg.disassemble(Memory::ROM_START, 4);
    EXPECT_EQ(instructions.size(), 4u);
}

TEST_F(DebuggerTest, GetCurrentInstructionDoesNotThrow) {
    Debugger dbg(emulator);
    EXPECT_NO_THROW({
        auto instr = dbg.get_current_instruction();
        (void)instr;
    });
}

TEST_F(DebuggerTest, GetCpuStateDoesNotThrow) {
    Debugger dbg(emulator);
    EXPECT_NO_THROW({
        auto state = dbg.get_cpu_state();
        (void)state;
    });
}
