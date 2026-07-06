#ifndef CHIP_8_MEMORY_VIEWER_H
#define CHIP_8_MEMORY_VIEWER_H

#include <cstdint>
#include <vector>

#include "core/Memory.h"


class MemoryViewer {
public:
  [[nodiscard]] static uint8_t peek(const Memory& memory, uint16_t addr);

  [[nodiscard]] static std::vector<uint8_t> get_range(const Memory& memory, uint16_t start, uint16_t length);

  [[nodiscard]] static std::string hex_dump(const Memory& memory, uint16_t start, uint16_t length, uint8_t bytes_per_row = 16); 
};

#endif
