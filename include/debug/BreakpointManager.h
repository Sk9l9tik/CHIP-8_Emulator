#ifndef CHIP_8_BREAKPOINT_MANAGER_H
#define CHIP_8_BREAKPOINT_MANAGER_H

#include <cstdint>
#include <sys/types.h>
#include <unordered_set>


class BreakpointManager {
public:
  void add(uint16_t addr); 
  void remove(uint16_t addr);
  void toggle(uint16_t addr);
  bool has(uint16_t addr) const;
  void clear();

  const std::unordered_set<uint16_t>& all() const;


private:  
  std::unordered_set<uint16_t> breakpoints_;
};

#endif //CHIP_8_BREAKPOINT_MANAGER_H
