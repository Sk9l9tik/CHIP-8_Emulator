#include "debug/CPUViewer.h"


CPUState CPUViewer::snapshot(const CPU& cpu) {
  CPUState state{};

  state.PC = cpu.get_PC();
  state.I = cpu.get_index();

  state.SP = cpu.get_stack_pointer();

  state.DT = cpu.get_delay_timer();
  state.ST = cpu.get_sound_timer();

  for (int i = 0; i < 16; ++i)
      state.V[i] = cpu.get_register(i);

  for (int i = 0; i < 16; ++i)
      state.stack[i] = cpu.get_stack(i);

  return state;
}
