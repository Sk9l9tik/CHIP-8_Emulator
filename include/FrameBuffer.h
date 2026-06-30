#ifndef CHIP_8_EMULATOR_FRAMEBUFFER_H
#define CHIP_8_EMULATOR_FRAMEBUFFER_H

#include <cstdint>
#include <array>

class FrameBuffer{
public:
    static constexpr uint8_t WIDTH = 64;
    static constexpr uint8_t HEIGHT = 32;

    FrameBuffer();

    void clear_screen();
    bool draw_sprite(uint8_t x, uint8_t y, const uint8_t* sprite_data, uint8_t height);
    const std::array<bool, WIDTH*HEIGHT>& get_frame_buffer() const;
private:
    std::array<bool, WIDTH*HEIGHT> pixels{};
};

#endif //CHIP_8_EMULATOR_FRAMEBUFFER_H
