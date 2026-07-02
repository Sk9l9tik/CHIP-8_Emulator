#include "debug/Debugger.h"


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
