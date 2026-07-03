#include "core/FrameBuffer.h"


FrameBuffer::FrameBuffer(){
    pixels.fill(false);
}
void FrameBuffer::clear_screen() {
    pixels.fill(false);
}
bool FrameBuffer::draw_sprite(uint8_t x, uint8_t y, const uint8_t* sprite_data, uint8_t height) {
    bool collision = false;

    for(uint8_t row = 0; row < height; ++row){
        uint8_t sprite_byte = sprite_data[row];
        for(uint8_t col = 0; col < 8; ++col){
            if((sprite_byte & (0x80 >> col)) == 0) continue;

            uint8_t px = (x + col) % WIDTH;
            uint8_t py = (y + row) % HEIGHT;
            size_t idx = py * WIDTH + px;
            if(pixels.at(idx)) collision = true;
            pixels.at(idx) = !pixels.at(idx);
        }
    }
    return collision;
}
const std::array<bool, FrameBuffer::WIDTH*FrameBuffer::HEIGHT>& FrameBuffer::get_frame_buffer() const {
    return pixels;
}
