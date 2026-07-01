#include <iostream>

#include "core/CPU.h"


void CPU::print() {std::cout << "Hello World!";}
uint16_t CPU::fetch() { return 0;}
void CPU::execute(uint16_t opcode) {}
CPU::CPU(Memory &memory_, FrameBuffer &frame_buffer_): memory(memory_), frame_buffer(frame_buffer_) {}
void CPU::reset() {
    V.fill(0);
    index = 0;
    program_counter = Memory::ROM_START;
    stack.fill(0);
    stack_pointer = 0;
    delay_timer = 0;
    sound_timer = 0;
    keypad.fill(0);
    waiting_for_key = false;
    key_register = 0;
    frame_buffer.clear_screen();
}
void CPU::tick() {
    uint16_t opcode = fetch();
    execute(opcode);
}
void CPU::set_key_state(uint8_t key, bool pressed) {}
bool CPU::get_key_state(uint8_t key) const { return 0;}
void CPU::set_sound_timer(uint8_t value) {
    sound_timer = value;
}
void CPU::set_delay_timer(uint8_t value) {
    delay_timer = value;
}
uint8_t CPU::get_delay_timer() const { return delay_timer;}
void CPU::tick_timers() {
    if(delay_timer > 0) delay_timer--;
    if(sound_timer > 0) sound_timer--;
}
uint16_t CPU::get_PC() const { return program_counter; }
uint8_t CPU::get_register(uint8_t index) const { return V.at(index); }
uint16_t CPU::get_index() const { return index;}
uint8_t CPU::get_stack_pointer() const { return stack_pointer;}
uint16_t CPU::get_stack(uint8_t index) const { return stack.at(index);}
