#include "core/CHIP_8.h"


CHIP_8::CHIP_8(): cpu(memory, frame_buffer) {}
void CHIP_8::load_ROM(const std::string &path) {
    memory.load_ROM(path);
}
void CHIP_8::tick() {}
void CHIP_8::tick_timers() {}
CPU& CHIP_8::get_cpu() { return cpu;}
const FrameBuffer& CHIP_8::get_frame_buffer() const { return{};}
const Memory& CHIP_8::get_memory() const { return memory; }
