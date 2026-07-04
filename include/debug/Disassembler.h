#ifndef CHIP_8_DISASSEMBLER_H
#define CHIP_8_DISASSEMBLER_H

#include <cstdint>
#include <vector>
#include <string>

#include "core/Memory.h"

struct DisassembledInstruction {
  uint16_t address;
  uint16_t opcode;
  std::string mnemonic;

};

class Disassembler {
public:
  static std::string decode(uint16_t opcode);

  static DisassembledInstruction current_instruction(const Memory& memory, uint16_t pc);

  static std::vector<DisassembledInstruction> disassemble_range(const Memory& memory, uint16_t start_addr, uint16_t count);

};

#endif //CHIP_8_DISASSEMBLER_H

