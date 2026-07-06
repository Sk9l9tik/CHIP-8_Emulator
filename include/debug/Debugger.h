#ifndef CHIP_8_DEBUGGER_H
#define CHIP_8_DEBUGGER_H

#include "core/CHIP_8.h"
#include "debug/BreakpointManager.h"
#include "debug/Disassembler.h"
#include "debug/MemoryViewer.h"
#include <cstdint>


class Debugger {
public:
  Debugger(CHIP_8& emu);

  void pause() noexcept;
  void resume() noexcept;
  void step(); // 1 instruction

  bool is_paused() const noexcept;

  void update();

  //  Breakpoints control
  void set_breakpoint(uint16_t addr);
  void remove_breakpoint(uint16_t addr);
  void toggle_breakpoint(uint16_t addr);
  bool has_breakpoint(uint16_t addr) const;
  void clear_breakpoints();

  const BreakpointManager& get_breakpoints() const;

  // Disassembler control
  DisassembledInstruction get_current_instruction() const;
  std::vector<DisassembledInstruction> disassemble(uint16_t start, uint16_t count) const;

  // void print_disassembled(std::ios_base& (*base)(std::ios_base&), uint16_t start, uint16_t count) const; // 0_-

  // MemoryViewer control
  std::vector<uint8_t> memory_range(uint16_t start, uint16_t length) const;
  std::string memory_dump(uint16_t start, uint16_t length) const; 

private:
  CHIP_8& emulator;
  BreakpointManager breakpoints;
  Disassembler disassembler;
  bool paused{};
};
  
#endif //CHIP_8_DEBUGGER_H
