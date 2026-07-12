#include <gtest/gtest.h>

#include "core/Memory.h"

#include "debug/MemoryViewer.h"

class MemoryViewerTest : public ::testing::Test {
protected:
    Memory mem{};
};

TEST_F(MemoryViewerTest, PeekReturnsByteWrittenToMemory) {
    mem.write(0x300, 0x42);
    EXPECT_EQ(MemoryViewer::peek(mem, 0x300), 0x42);
}

TEST_F(MemoryViewerTest, PeekReturnsFontDataAtDefaultLocation) {
    EXPECT_EQ(MemoryViewer::peek(mem, Memory::FONT_START), 0xF0);
}

TEST_F(MemoryViewerTest, GetRangeReturnsRequestedLengthOfBytes) {
    mem.write(0x200, 0x11);
    mem.write(0x201, 0x22);
    mem.write(0x202, 0x33);

    auto range = MemoryViewer::get_range(mem, 0x200, 3);

    ASSERT_EQ(range.size(), 3u);
    EXPECT_EQ(range[0], 0x11);
    EXPECT_EQ(range[1], 0x22);
    EXPECT_EQ(range[2], 0x33);
}

TEST_F(MemoryViewerTest, GetRangeWithZeroLengthReturnsEmptyVector) {
    auto range = MemoryViewer::get_range(mem, 0x200, 0);
    EXPECT_TRUE(range.empty());
}

TEST_F(MemoryViewerTest, GetRangeClipsAtEndOfMemory) {
    auto range = MemoryViewer::get_range(mem, 0x0FF0, 32);

    EXPECT_EQ(range.size(), 16u);
}

TEST_F(MemoryViewerTest, GetRangeStartingAtOrBeyondMemSizeReturnsEmptyVector) {
    auto range = MemoryViewer::get_range(mem, 4096, 10);
    EXPECT_TRUE(range.empty());
}

TEST_F(MemoryViewerTest, HexDumpFormatsSingleRowWithDefaultBytesPerRow) {
    mem.write(0x200, 0x12);
    mem.write(0x201, 0xAB);
    mem.write(0x202, 0x00);

    std::string dump = MemoryViewer::hex_dump(mem, 0x200, 3);

    EXPECT_EQ(dump, "0x0200:  12 AB 00 \n");
}

TEST_F(MemoryViewerTest, HexDumpSplitsIntoMultipleRowsAccordingToBytesPerRow) {
    mem.write(0x200, 0x01);
    mem.write(0x201, 0x02);
    mem.write(0x202, 0x03);
    mem.write(0x203, 0x04);

    std::string dump = MemoryViewer::hex_dump(mem, 0x200, 4, 2);

    EXPECT_EQ(dump, "0x0200:  01 02 \n0x0202:  03 04 \n");
}

TEST_F(MemoryViewerTest, HexDumpLastRowIsPartialWhenLengthNotMultipleOfBytesPerRow) {
    mem.write(0x200, 0x01);
    mem.write(0x201, 0x02);
    mem.write(0x202, 0x03);

    std::string dump = MemoryViewer::hex_dump(mem, 0x200, 3, 2);

    EXPECT_EQ(dump, "0x0200:  01 02 \n0x0202:  03 \n");
}

TEST_F(MemoryViewerTest, HexDumpWithZeroBytesPerRowDefaultsToSixteen) {
    for (uint16_t i = 0; i < 16; ++i)
        mem.write(0x200 + i, 0xAA);

    std::string dump_zero = MemoryViewer::hex_dump(mem, 0x200, 16, 0);
    std::string dump_sixteen = MemoryViewer::hex_dump(mem, 0x200, 16, 16);

    EXPECT_EQ(dump_zero, dump_sixteen);
}

TEST_F(MemoryViewerTest, HexDumpOnEmptyRangeReturnsEmptyString) {
    std::string dump = MemoryViewer::hex_dump(mem, 0x200, 0);
    EXPECT_TRUE(dump.empty());
}
