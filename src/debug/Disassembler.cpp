#include <iomanip>
#include <sstream>
#include <vector>

#include "debug/Disassembler.h"

#include "core/Memory.h"

std::string Disassembler::decode(uint16_t opcode) {
  uint8_t C = (opcode & 0xF000) >> 12;
  uint8_t X = (opcode & 0x0F00) >> 8;
  uint8_t Y = (opcode & 0x00F0) >> 4;
  uint8_t N = opcode & 0x000F;
  uint8_t NN = opcode & 0x00FF;
  uint16_t NNN = opcode & 0x0FFF;

  std::ostringstream oss;
  oss << std::uppercase << std::hex;

  switch (C) {
  case 0x0:
    switch (NN) {
    case 0xE0:
      return "CLS";
    case 0xEE:
      return "RET";
    default:
      oss << "DW 0x" << std::setw(4) << std::setfill('0') << opcode;
      return oss.str();
    }
  case 0x1:
    oss << "JMP 0x" << std::setw(3) << std::setfill('0') << NNN;
    return oss.str();
  case 0x2:
    oss << "CALL 0x" << std::setw(3) << std::setfill('0') << NNN;
    return oss.str();
  case 0x3:
    oss << "SE V" << static_cast<int>(X) << ", 0x" << std::setw(2)
        << std::setfill('0') << static_cast<int>(NN);
    return oss.str();
  case 0x4:
    oss << "SNE V" << static_cast<int>(X) << ", 0x" << std::setw(2)
        << std::setfill('0') << static_cast<int>(NN);
    return oss.str();
  case 0x5:
    oss << "SE V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
    return oss.str();
  case 0x6:
    oss << "LD V" << static_cast<int>(X) << ", 0x" << std::setw(2)
        << std::setfill('0') << static_cast<int>(NN);
    return oss.str();
  case 0x7:
    oss << "ADD V" << static_cast<int>(X) << ", 0x" << std::setw(2)
        << std::setfill('0') << static_cast<int>(NN);
    return oss.str();
  case 0x8:
    switch (N) {
    case 0x0:
      oss << "LD V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
      return oss.str();
    case 0x1:
      oss << "OR V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
      return oss.str();
    case 0x2:
      oss << "AND V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
      return oss.str();
    case 0x3:
      oss << "XOR V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
      return oss.str();
    case 0x4:
      oss << "ADD V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
      return oss.str();
    case 0x5:
      oss << "SUB V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
      return oss.str();
    case 0x6:
      oss << "SHR V" << static_cast<int>(X);
      return oss.str();
    case 0x7:
      oss << "SUBN V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
      return oss.str();
    case 0xE:
      oss << "SHL V" << static_cast<int>(X);
      return oss.str();
    default:
      oss << "DW 0x" << std::setw(4) << std::setfill('0') << opcode;
      return oss.str();
    }
  case 0x9:
    oss << "SNE V" << static_cast<int>(X) << ", V" << static_cast<int>(Y);
    return oss.str();
  case 0xA:
    oss << "LD I, 0x" << std::setw(3) << std::setfill('0') << NNN;
    return oss.str();
  case 0xB:
    oss << "JMP V0, 0x" << std::setw(3) << std::setfill('0') << NNN;
    return oss.str();
  case 0xC:
    oss << "RND V" << static_cast<int>(X) << ", 0x" << std::setw(2)
        << std::setfill('0') << static_cast<int>(NN);
    return oss.str();
  case 0xD:
    oss << "DRW V" << static_cast<int>(X) << ", V" << static_cast<int>(Y)
        << ", 0x" << static_cast<int>(N);
    return oss.str();
  case 0xE:
    switch (NN) {
    case 0x9E:
      oss << "SKP V" << static_cast<int>(X);
      return oss.str();
    case 0xA1:
      oss << "SKNP V" << static_cast<int>(X);
      return oss.str();
    default:
      oss << "DW 0x" << std::setw(4) << std::setfill('0') << opcode;
      return oss.str();
    }
  case 0xF:
    switch (NN) {
    case 0x07:
      oss << "LD V" << static_cast<int>(X) << ", DT";
      return oss.str();
    case 0x0A:
      oss << "LD V" << static_cast<int>(X) << ", K";
      return oss.str();
    case 0x15:
      oss << "LD DT, V" << static_cast<int>(X);
      return oss.str();
    case 0x18:
      oss << "LD ST, V" << static_cast<int>(X);
      return oss.str();
    case 0x1E:
      oss << "ADD I, V" << static_cast<int>(X);
      return oss.str();
    case 0x29:
      oss << "LD F, V" << static_cast<int>(X);
      return oss.str();
    case 0x33:
      oss << "LD B, V" << static_cast<int>(X);
      return oss.str();
    case 0x55:
      oss << "LD [I], V" << static_cast<int>(X);
      return oss.str();
    case 0x65:
      oss << "LD V" << static_cast<int>(X) << ", [I]";
      return oss.str();
    default:
      oss << "DW 0x" << std::setw(4) << std::setfill('0') << opcode;
      return oss.str();
    }
  default:
    oss << "DW 0x" << std::setw(4) << std::setfill('0') << opcode;
    return oss.str();
  }
}

DisassembledInstruction Disassembler::current_instruction(const Memory &memory,
                                                          uint16_t pc) {
  uint16_t opcode =
      (static_cast<uint16_t>(memory.read(pc)) << 8) | memory.read(pc + 1);
  return DisassembledInstruction{pc, opcode, decode(opcode)};
}

std::vector<DisassembledInstruction>
Disassembler::disassemble_range(const Memory &memory, uint16_t start_addr,
                                uint16_t count) {
  std::vector<DisassembledInstruction> result;
  result.reserve(count);

  uint16_t addr = start_addr;
  for (uint16_t i = 0; i < count; ++i) {
    result.emplace_back(current_instruction(memory, addr));
    addr += 2;
  }
  return result;
}
