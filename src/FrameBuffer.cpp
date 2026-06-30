#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() = default;
void FrameBuffer::clear_screen() {}
bool FrameBuffer::draw_sprite(uint8_t x, uint8_t y, const uint8_t *sprite_data, uint8_t height) { return false;}
const std::array<bool, FrameBuffer::WIDTH*FrameBuffer::HEIGHT>& FrameBuffer::get_frame_buffer() const {
    const std::array<bool, FrameBuffer::WIDTH*FrameBuffer::HEIGHT>& prikol{};
    return prikol;
}