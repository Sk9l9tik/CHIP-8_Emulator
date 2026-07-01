#ifndef CHIP_8_EMULATOR_MEMORY_H
#define CHIP_8_EMULATOR_MEMORY_H

#include <cstdint>
#include <string>
#include <array>


class Memory{
public:
    static constexpr uint16_t ROM_START = 0x200;
    static constexpr uint16_t FONT_START = 0x050;
    static constexpr uint16_t MEM_SIZE = 4096;

    Memory(); //надо будет шрифт зафигарить в 0x050 CHTO

    void load_ROM(const std::string& path);
    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t val);

    //Dlya MemoryViewer наверно
    const uint8_t* raw() const;
private:
    std::array<uint8_t, MEM_SIZE> data{};
    void load_font();
};
#endif //CHIP_8_EMULATOR_MEMORY_H
