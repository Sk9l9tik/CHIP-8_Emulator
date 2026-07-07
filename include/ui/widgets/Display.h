#ifndef CHIP_8_EMULATOR_DISPLAY_H
#define CHIP_8_EMULATOR_DISPLAY_H

#include "ui/Widget.h"

class Display : public Widget{
public:
    static constexpr unsigned char DISPLAY_WIDTH = 64;
    static constexpr unsigned char DISPLAY_HEIGHT = 32;

    explicit Display(const FrameBuffer &data) : pixel_data(data) {
        d_texture.clear();
        d_texture.display();

        update_texture();

        //d_sprite.setTexture(d_texture.getTexture());
    }

    ~Display() override = default;

    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i &mouse_pos) override {
        // ну неважно в принципе
    }

    void update() override { // че
        update_texture();
    }

    void render(sf::RenderTarget &target) override {
        target.draw(d_sprite);
    }

    void set_pixel_size(float p_size){
        pixel_size = p_size;
    }
private:
    void update_texture(){
        d_texture.clear(sf::Color::Black);

        sf::VertexArray pixels(sf::PrimitiveType::Points, DISPLAY_WIDTH * DISPLAY_HEIGHT);

        for (unsigned int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
            pixels[i].position = {
                    static_cast<float>(i % DISPLAY_WIDTH),
                    static_cast<float>(i / DISPLAY_WIDTH)
            };
            pixels[i].color = pixel_data.get_frame_buffer()[i] ?
                              sf::Color::Black : sf::Color::White;
        }

        d_texture.draw(pixels);
        d_texture.display();
        d_sprite.setTexture(d_texture.getTexture());
        d_sprite.setPosition(pos);
        d_sprite.setScale({pixel_size, pixel_size});
    }

    sf::RenderTexture d_texture{{DISPLAY_WIDTH, DISPLAY_HEIGHT}};
    sf::Sprite d_sprite{d_texture.getTexture()};
    const FrameBuffer &pixel_data;
    float pixel_size = 10.f;
};
#endif //CHIP_8_EMULATOR_DISPLAY_H
