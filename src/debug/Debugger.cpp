#include "debug/Debugger.h"

#include "debug/CPUViewer.h"
#include "debug/MemoryViewer.h"

Debugger::Debugger(CHIP_8& emu) : emulator_(emu) {}

void Debugger::step() { emulator_.tick(); }

void Debugger::pause() noexcept { paused_ = true; }
void Debugger::resume() noexcept { paused_ = false; }

bool Debugger::is_paused() const noexcept { return paused_; }

void Debugger::update() {
  if (paused_)
    return;

  if (breakpoints_.has(emulator_.get_cpu().get_PC())) {
    pause();
    return;
  }

  emulator_.tick();
}

void Debugger::set_breakpoint(uint16_t addr) { breakpoints_.add(addr); }
void Debugger::remove_breakpoint(uint16_t addr) { breakpoints_.remove(addr); }
void Debugger::toggle_breakpoint(uint16_t addr) { breakpoints_.toggle(addr); }
bool Debugger::has_breakpoint(uint16_t addr) const {
  return breakpoints_.has(addr);
}
void Debugger::clear_breakpoints() { breakpoints_.clear(); }

const BreakpointManager &Debugger::get_breakpoints() const {
  return breakpoints_;
}

DisassembledInstruction Debugger::get_current_instruction() const {
  return Disassembler::current_instruction(emulator_.get_memory(),
                                           emulator_.get_cpu().get_PC());
}
std::vector<DisassembledInstruction>
Debugger::disassemble(uint16_t start, uint16_t count) const {
  return Disassembler::disassemble_range(emulator_.get_memory(), start, count);
}

std::vector<uint8_t> Debugger::memory_range(uint16_t start,
                                            uint16_t length) const {
  return MemoryViewer::get_range(emulator_.get_memory(), start, length);
}

std::string Debugger::memory_dump(uint16_t start, uint16_t length) const {
  return MemoryViewer::hex_dump(emulator_.get_memory(), start, length);
}

CPUState Debugger::get_cpu_state() const noexcept {
  return CPUViewer::snapshot(emulator_.get_cpu());
}
