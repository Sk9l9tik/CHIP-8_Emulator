#include "Memory.h"
#include <fstream>
#include <stdexcept>

static const uint8_t FONT_DATA[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Memory::Memory(){
    load_font();
}
void Memory::load_ROM(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if(!file.is_open()) throw std::runtime_error("Failed to open ROM: " + path);

    std::streamsize file_size = file.tellg();
    if(file_size > static_cast<std::streamsize>(MEM_SIZE - ROM_START))
        throw std::runtime_error("ROM too large: " + path);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(data.data() + ROM_START), file_size);
}
uint8_t Memory::read(uint16_t addr) const {
    return data.at(addr);
}
void Memory::write(uint16_t addr, uint8_t val) {
    data.at(addr) = val;
}
const uint8_t* Memory::raw() const {
    return data.data();
}
void Memory::load_font() {
    for(int i = 0; i < 80; ++i)
        data.at(FONT_START + i) = FONT_DATA[i];
}