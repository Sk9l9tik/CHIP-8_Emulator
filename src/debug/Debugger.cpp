#include <iterator>

#include "debug/Debugger.h"

#include "debug/MemoryViewer.h"

Debugger::Debugger(CHIP_8& emu) : emulator(emu) {}

void Debugger::step(){ emulator.tick(); }

void Debugger::pause() noexcept { paused = true; }
void Debugger::resume() noexcept { paused = false; }

bool Debugger::is_paused() const noexcept { return paused; }

void Debugger::update() {
  if(paused) return;

  if(breakpoints.has(emulator.get_cpu().get_PC())) {
    pause();
    return;
  }

  emulator.tick();
}

void Debugger::set_breakpoint(uint16_t addr) { breakpoints.add(addr); }
void Debugger::remove_breakpoint(uint16_t addr) { breakpoints.remove(addr); }
void Debugger::toggle_breakpoint(uint16_t addr) { breakpoints.toggle(addr); }
bool Debugger::has_breakpoint(uint16_t addr) const { return breakpoints.has(addr); }
void Debugger::clear_breakpoints() { breakpoints.clear(); }

const BreakpointManager& Debugger::get_breakpoints() const { return breakpoints; }


DisassembledInstruction Debugger::get_current_instruction() const {
  return Disassembler::current_instruction(emulator.get_memory(), emulator.get_cpu().get_PC());
}
std::vector<DisassembledInstruction> Debugger::disassemble(uint16_t start, uint16_t count) const {
  return Disassembler::disassemble_range(emulator.get_memory(), start, count);
} 


std::vector<uint8_t> Debugger::memory_range(uint16_t start, uint16_t length) const {
  return MemoryViewer::get_range(emulator.get_memory(), start, length);
}

std::string Debugger::memory_dump(uint16_t start, uint16_t length) const {
  return MemoryViewer::hex_dump(emulator.get_memory(), start, length);
}
