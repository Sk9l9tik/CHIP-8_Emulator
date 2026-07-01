#ifndef CHIP_8_DEBUGGER_H
#define CHIP_8_DEBUGGER_H

#include <memory>

#include "core/CPU.h"


class Debugger {
  std::unique_ptr<CPU> cpu;
  bool paused{};
  
public:
  void pause();
  void resume();
  void step(); // 1 instruction

  bool is_paused();
};

#endif //CHIP_8_DEBUGGER_H
