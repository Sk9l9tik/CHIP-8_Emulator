#include <sys/types.h>

#include "debug/BreakpointManager.h"


void BreakpointManager::add(uint16_t addr) {
  breakpoints_.insert(addr);
}

void BreakpointManager::remove(uint16_t addr) {
  breakpoints_.erase(addr);
}

void BreakpointManager::toggle(uint16_t addr) {
  if (has(addr))
    breakpoints_.erase(addr);
  else
    breakpoints_.insert(addr);
}

void BreakpointManager::clear(){
  breakpoints_.clear();
}


bool BreakpointManager::has(uint16_t addr) const {
  return breakpoints_.find(addr) != breakpoints_.end();
}

const std::unordered_set<uint16_t>& BreakpointManager::all() const {
  return breakpoints_;
}

