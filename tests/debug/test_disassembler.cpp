#include <gtest/gtest.h>

#include "core/Memory.h"

#include "debug/Disassembler.h"

class DisassemblerTest : public ::testing::Test {
protected:
    Disassembler disassembler;
};

//
// decode()
//

TEST_F(DisassemblerTest, DecodeCls)
{
    EXPECT_EQ(disassembler.decode(0x00E0), "CLS");
}

TEST_F(DisassemblerTest, DecodeRet)
{
    EXPECT_EQ(disassembler.decode(0x00EE), "RET");
}

TEST_F(DisassemblerTest, DecodeJump)
{
    EXPECT_EQ(disassembler.decode(0x1234), "JMP 0x234");
}

TEST_F(DisassemblerTest, DecodeCall)
{
    EXPECT_EQ(disassembler.decode(0x2456), "CALL 0x456");
}

TEST_F(DisassemblerTest, DecodeSeImmediate)
{
    EXPECT_EQ(disassembler.decode(0x3AFF), "SE VA, 0xFF");
}

TEST_F(DisassemblerTest, DecodeSneImmediate)
{
    EXPECT_EQ(disassembler.decode(0x4B12), "SNE VB, 0x12");
}

TEST_F(DisassemblerTest, DecodeSeRegister)
{
    EXPECT_EQ(disassembler.decode(0x5120), "SE V1, V2");
}

TEST_F(DisassemblerTest, DecodeLdImmediate)
{
    EXPECT_EQ(disassembler.decode(0x6CFE), "LD VC, 0xFE");
}

TEST_F(DisassemblerTest, DecodeAddImmediate)
{
    EXPECT_EQ(disassembler.decode(0x7D01), "ADD VD, 0x01");
}

TEST_F(DisassemblerTest, DecodeArithmeticInstructions)
{
    EXPECT_EQ(disassembler.decode(0x8120), "LD V1, V2");
    EXPECT_EQ(disassembler.decode(0x8121), "OR V1, V2");
    EXPECT_EQ(disassembler.decode(0x8122), "AND V1, V2");
    EXPECT_EQ(disassembler.decode(0x8123), "XOR V1, V2");
    EXPECT_EQ(disassembler.decode(0x8124), "ADD V1, V2");
    EXPECT_EQ(disassembler.decode(0x8125), "SUB V1, V2");
    EXPECT_EQ(disassembler.decode(0x8126), "SHR V1");
    EXPECT_EQ(disassembler.decode(0x8127), "SUBN V1, V2");
    EXPECT_EQ(disassembler.decode(0x812E), "SHL V1");
}

TEST_F(DisassemblerTest, DecodeSneRegister)
{
    EXPECT_EQ(disassembler.decode(0x9120), "SNE V1, V2");
}

TEST_F(DisassemblerTest, DecodeLdI)
{
    EXPECT_EQ(disassembler.decode(0xAABC), "LD I, 0xABC");
}

TEST_F(DisassemblerTest, DecodeJumpV0)
{
    EXPECT_EQ(disassembler.decode(0xB123), "JMP V0, 0x123");
}

TEST_F(DisassemblerTest, DecodeRnd)
{
    EXPECT_EQ(disassembler.decode(0xC4AB), "RND V4, 0xAB");
}

TEST_F(DisassemblerTest, DecodeDraw)
{
    EXPECT_EQ(disassembler.decode(0xD125), "DRW V1, V2, 0x5");
}

TEST_F(DisassemblerTest, DecodeKeyboardInstructions)
{
    EXPECT_EQ(disassembler.decode(0xE19E), "SKP V1");
    EXPECT_EQ(disassembler.decode(0xE2A1), "SKNP V2");
}

TEST_F(DisassemblerTest, DecodeFInstructions)
{
    EXPECT_EQ(disassembler.decode(0xF107), "LD V1, DT");
    EXPECT_EQ(disassembler.decode(0xF10A), "LD V1, K");
    EXPECT_EQ(disassembler.decode(0xF115), "LD DT, V1");
    EXPECT_EQ(disassembler.decode(0xF118), "LD ST, V1");
    EXPECT_EQ(disassembler.decode(0xF11E), "ADD I, V1");
    EXPECT_EQ(disassembler.decode(0xF129), "LD F, V1");
    EXPECT_EQ(disassembler.decode(0xF133), "LD B, V1");
    EXPECT_EQ(disassembler.decode(0xF155), "LD [I], V1");
    EXPECT_EQ(disassembler.decode(0xF165), "LD V1, [I]");
}

TEST_F(DisassemblerTest, DecodeUnknownInstructionReturnsDw)
{
    EXPECT_EQ(disassembler.decode(0x8128), "DW 0x8128");
    EXPECT_EQ(disassembler.decode(0x00FF), "DW 0x00FF");
    EXPECT_EQ(disassembler.decode(0xE1FF), "DW 0xE1FF");
    EXPECT_EQ(disassembler.decode(0xF1FF), "DW 0xF1FF");
}

//
// current_instruction()
//

TEST_F(DisassemblerTest, CurrentInstructionReadsMemory)
{
    Memory memory;

    memory.write(0x200, 0x61);
    memory.write(0x201, 0x23);

    auto inst = disassembler.current_instruction(memory, 0x200);

    EXPECT_EQ(inst.address, 0x200);
    EXPECT_EQ(inst.opcode, 0x6123);
    EXPECT_EQ(inst.mnemonic, "LD V1, 0x23");
}

//
// disassemble_range()
//

TEST_F(DisassemblerTest, DisassembleRange)
{
    Memory memory;

    memory.write(0x200, 0x00);
    memory.write(0x201, 0xE0);

    memory.write(0x202, 0x61);
    memory.write(0x203, 0x01);

    memory.write(0x204, 0x12);
    memory.write(0x205, 0x34);

    auto instructions = disassembler.disassemble_range(memory, 0x200, 3);

    ASSERT_EQ(instructions.size(), 3);

    EXPECT_EQ(instructions[0].address, 0x200);
    EXPECT_EQ(instructions[0].opcode, 0x00E0);
    EXPECT_EQ(instructions[0].mnemonic, "CLS");

    EXPECT_EQ(instructions[1].address, 0x202);
    EXPECT_EQ(instructions[1].opcode, 0x6101);
    EXPECT_EQ(instructions[1].mnemonic, "LD V1, 0x01");

    EXPECT_EQ(instructions[2].address, 0x204);
    EXPECT_EQ(instructions[2].opcode, 0x1234);
    EXPECT_EQ(instructions[2].mnemonic, "JMP 0x234");
}
