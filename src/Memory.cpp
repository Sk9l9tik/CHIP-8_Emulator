#include "Memory.h"

Memory::Memory() = default;
void Memory::load_ROM(const std::string &path) {}
uint8_t Memory::read(uint16_t addr) const { return 0;}
void Memory::write(uint16_t addr, uint8_t val) {}
const uint8_t* Memory::raw() const {return nullptr;}
void Memory::load_font() {}