#include <iostream>
#include "CPU.h"

void CPU::print() {std::cout << "Hello World!";}
uint16_t CPU::fetch() { return 0;}
void CPU::execute(uint16_t opcode) {}
CPU::CPU(Memory &memory_, FrameBuffer &frame_buffer_): memory(memory_), frame_buffer(frame_buffer_) {}
void CPU::reset() {}
void CPU::tick() {}
void CPU::set_key_state(uint8_t key, bool pressed) {}
bool CPU::get_key_state(uint8_t key) const { return 0;}
void CPU::set_sound_timer(uint8_t value) {}
void CPU::set_delay_timer(uint8_t value) {}
uint8_t CPU::get_delay_timer() const { return 0;}
void CPU::tick_timers() {}
uint16_t CPU::get_PC() const { return 0; }
uint8_t CPU::get_register(uint8_t index) const { return 0;}
uint16_t CPU::get_index() const { return 0;}
uint8_t CPU::get_stack_pointer() const { return 1;}
uint16_t CPU::get_stack(uint8_t index) const { return 0;}