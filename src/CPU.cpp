#include <iostream>
#include "CPU.h"

void CPU::print() {std::cout << "Hello World!";}
uint16_t CPU::fetch() { return 0;}
void CPU::execute(uint16_t opcode) {}
void CPU::tick_timers() {}
void CPU::reset() {}
void CPU::tick() {}
void CPU::set_key_state(uint8_t key, bool pressed) {}
uint8_t CPU::get_sound_timer() const { return 0;}
bool CPU::is_key_pressed(uint8_t key) const {return 0;}
uint16_t CPU::get_PC() const { return 0; }
uint8_t CPU::get_register(uint8_t index) const { return 0;}
uint16_t CPU::get_index() const { return 0;}
uint8_t CPU::get_stack_pointer() const { return 1;}
uint16_t CPU::get_stack(uint8_t index) const { return 0;}
uint8_t CPU::get_delay_timer() const { return 0;}
