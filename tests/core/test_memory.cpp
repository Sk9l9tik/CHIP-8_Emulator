#include <gtest/gtest.h>
#include <fstream>
#include "core/Memory.h"

void create_test_ROM(const std::string& path, const std::vector<uint8_t>& data){
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

TEST(Memory, FontLoadedAtCorrectAddress){
    Memory mem{};

    EXPECT_EQ(mem.read(0x050), 0xF0);
    EXPECT_EQ(mem.read(0x051), 0x90);
    EXPECT_EQ(mem.read(0x054), 0xF0);
}

TEST(Memory, FontLoadedAtCorrectAddressEnd) {
    Memory mem{};

    EXPECT_EQ(mem.read(0x9B), 0xF0);
    EXPECT_EQ(mem.read(0x9C), 0x80);
    EXPECT_EQ(mem.read(0x9F), 0x80);
}

TEST(Memory, SpaceBeforeFontsIsEmptyAfterConstructor){
    Memory mem{};

    for(uint16_t i = 0x0; i < 0x050; ++i){
        EXPECT_EQ(mem.read(i), 0x0);
    }
}

TEST(Memory, LoadROMCorrectly){
    Memory mem{};

    std::vector<uint8_t> rom_data {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
    std::string test_path {"test_load.ch8"};
    create_test_ROM(test_path, rom_data);

    EXPECT_NO_THROW(mem.load_ROM(test_path));

    for(uint16_t i = 0; i < rom_data.size(); ++i)
        EXPECT_EQ(mem.read(Memory::ROM_START + i), rom_data.at(i));

    std::remove(test_path.c_str());
}

TEST(Memory, LoadROMWithIncorrectPath){
    Memory mem{};

    std::string wrong_path {"prikol.ch8"};
    EXPECT_THROW(mem.load_ROM(wrong_path), std::runtime_error);
}

TEST(Memory, LoadROMWithTooMuchSize){
    Memory mem{};

    std::vector<uint8_t> test_data(Memory::MEM_SIZE, 0x00);
    std::string test_path {"prikol.ch8"};
    create_test_ROM(test_path, test_data);

    EXPECT_THROW(mem.load_ROM(test_path), std::runtime_error);

    std::remove(test_path.c_str());
}

TEST(Memory, WriteAndReadOnSameAddress){
    Memory mem{};

    mem.write(0x200, 0x78);
    EXPECT_EQ(mem.read(0x200), 0x78);
}

TEST(Memory, WriteAndReadOnInvalidAddress){
    Memory mem{};

    EXPECT_THROW(mem.write(0x1000, 0x78), std::out_of_range);
    EXPECT_THROW(static_cast<void>(mem.read(0x1000)), std::out_of_range);
}