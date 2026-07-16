#ifndef CHIP_8_DEBUGGER_H
#define CHIP_8_DEBUGGER_H

#include <cstdint>

#include "core/CHIP_8.h"
#include "debug/BreakpointManager.h"
#include "debug/Disassembler.h"
#include "debug/CPUViewer.h"


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

  [[nodiscard]] const BreakpointManager& get_breakpoints() const;

  // Disassembler control
  [[nodiscard]] DisassembledInstruction get_current_instruction() const;
  [[nodiscard]] std::vector<DisassembledInstruction> disassemble(uint16_t start, uint16_t count) const;

  // MemoryViewer control
  [[nodiscard]] std::vector<uint8_t> memory_range(uint16_t start, uint16_t length) const;
  [[nodiscard]] std::string memory_dump(uint16_t start, uint16_t length) const; 

  // CPUViewer control
  [[nodiscard]] CPUState get_cpu_state() const noexcept;

private:
  CHIP_8& emulator_;
  BreakpointManager breakpoints_;
  Disassembler disassembler_;
  bool paused_{};
};
  
#endif //CHIP_8_DEBUGGER_H
