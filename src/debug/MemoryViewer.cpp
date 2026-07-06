#include <cstdint>

#include <iomanip>
#include <sstream>
#include <vector>

#include "debug/MemoryViewer.h"


uint8_t MemoryViewer::peek(const Memory& memory, uint16_t addr) {
  return memory.read(addr);
}

std::vector<uint8_t> MemoryViewer::get_range(const Memory& memory, uint16_t start, uint16_t length) {
  std::vector<uint8_t> result;
  result.reserve(length);

  for (uint16_t i = 0; i < length; ++i) {

    uint32_t addr = static_cast<uint32_t>(start) + i;

    if (addr >= 4096) {
        break;
    }

    result.push_back(memory.read(static_cast<uint16_t>(addr)));
  } 

  return result;
}

std::string MemoryViewer::hex_dump(const Memory& memory, uint16_t start, uint16_t length, uint8_t bytes_per_row) {
  if (bytes_per_row == 0) bytes_per_row = 16; 

  std::ostringstream oss;
  auto bytes = get_range(memory, start, length);

  for (size_t i = 0, end = bytes.size(); i < end; i += bytes_per_row) {
    oss << "0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << (start + i) << ":  ";
    
    size_t row_end = std::min(i + bytes_per_row, bytes.size());

    for (size_t j = i; j < row_end; ++j) {
      oss << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[j]) << ' ';
    }
    oss << std::dec << '\n';
  }
  return oss.str();
}
