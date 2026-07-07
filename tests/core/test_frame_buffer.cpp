#include <gtest/gtest.h>
#include "core/FrameBuffer.h"

TEST(FrameBuffer, AllPixelsAreFalseAfterConstructorAndClearScreen){
    FrameBuffer fb{};

    for(const auto& prikol: fb.get_frame_buffer())
        EXPECT_FALSE(prikol);

    uint8_t sprite_data[] {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    static_cast<void>(fb.draw_sprite(0,0, sprite_data, 5));
    fb.clear_screen();

    for(const auto& prikol: fb.get_frame_buffer())
        EXPECT_FALSE(prikol);
}

TEST(FrameBuffer, DrawCorrectPosAndReturnCollisionAndXOR){
    FrameBuffer fb{};

    uint8_t sprite_data[] {0x80};
    EXPECT_FALSE(fb.draw_sprite(0,0,sprite_data, 1));
    const auto& pixels {fb.get_frame_buffer()};

    EXPECT_TRUE(pixels.at(0));
    EXPECT_FALSE(pixels.at(1));

    EXPECT_TRUE(fb.draw_sprite(0,0, sprite_data, 1));

    EXPECT_FALSE(pixels.at(0));
}

TEST(FrameBuffer, DrawWrapX){
    FrameBuffer fb{};

    uint8_t sprite_data[] {0xFF, 0xFF};
    static_cast<void>(fb.draw_sprite(60,0, sprite_data, 2));

    const auto& pixels {fb.get_frame_buffer()};

    EXPECT_TRUE(pixels.at(60));
    EXPECT_TRUE(pixels.at(63));
    EXPECT_TRUE(pixels.at(0));
    EXPECT_TRUE(pixels.at(3));
}

TEST(FrameBuffer, DrawWrapY){
    FrameBuffer fb{};

    uint8_t sprite_data[] {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    static_cast<void>(fb.draw_sprite(0,30, sprite_data,5));

    const auto& pixels {fb.get_frame_buffer()};

    EXPECT_TRUE(pixels.at(30*FrameBuffer::WIDTH));
    EXPECT_TRUE(pixels.at(31*FrameBuffer::WIDTH));
    EXPECT_TRUE(pixels.at(0*FrameBuffer::WIDTH));
    EXPECT_TRUE(pixels.at(2*FrameBuffer::WIDTH));
}