#include <sys/types.h>

#include "debug/BreakpointManager.h"


void BreakpointManager::add(uint16_t addr) {
  breakpoints.insert(addr);
}

void BreakpointManager::remove(uint16_t addr) {
  breakpoints.erase(addr);
}

void BreakpointManager::toggle(uint16_t addr) {
  if (has(addr))
    breakpoints.erase(addr);
  else
    breakpoints.insert(addr);
}

void BreakpointManager::clear(){
  breakpoints.clear();
}


bool BreakpointManager::has(uint16_t addr) const {
  return breakpoints.find(addr) != breakpoints.end();
}

const std::unordered_set<uint16_t>& BreakpointManager::all() const {
  return breakpoints;
}

