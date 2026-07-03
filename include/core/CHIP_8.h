#ifndef CHIP_8_EMULATOR_CHIP_8_H
#define CHIP_8_EMULATOR_CHIP_8_H

#include "Memory.h"
#include "FrameBuffer.h"
#include "CPU.h"


class CHIP_8{
    CPU cpu;
    Memory memory;
    FrameBuffer frame_buffer;
public:
    CHIP_8();

    void load_ROM(const std::string& path);
    void tick();
    void tick_timers();

    [[nodiscard]] CPU& get_cpu();
    //read-only приколы. только CPU их может менять, а остальные только читать
    [[nodiscard]] const FrameBuffer& get_frame_buffer() const;
    [[nodiscard]] const Memory& get_memory() const;
};

#endif //CHIP_8_EMULATOR_CHIP_8_H
