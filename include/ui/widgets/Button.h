#ifndef CHIP_8_EMULATOR_BUTTON_H
#define CHIP_8_EMULATOR_BUTTON_H

#include "ui/Widget.h"

class Button : public Widget{
public:


    Button(std::string btn_text, sf::Vector2f size, sf::Vector2f pos, sf::Font &btn_font) : font(btn_font){
        text = sf::Text(font, btn_text, 16);

        bg_texture = sf::RenderTexture({static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)});
        bg_texture.clear(sf::Color::Transparent);

        sf::RectangleShape rect(size);
        rect.setFillColor(sf::Color::Red);
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(3);

        bg_texture.draw(rect);
        bg_texture.draw(text);

        bg_texture.display();

        background.setTexture(bg_texture.getTexture());
        background.setPosition(pos);
    }

    ~Button() override = default;

    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i &mouse_pos) override {

    }

    void update() override {

    }

    void render(sf::RenderTarget &target) override {

    }

    bool load_texture(std::string name, std::string path){

    }

    void lock();
    void unlock();

    bool is_locked() const { return state == State::Locked; }
private:
    enum class State{
        Undefined,
        Normal,
        Locked,
        Hovered,
        Pressed,
    };

    State state = State::Undefined;

    std::unordered_map<std::string, sf::Image> textures;

    sf::RenderTexture bg_texture    = sf::RenderTexture{};
    sf::Sprite background           = sf::Sprite{bg_texture.getTexture()};
    sf::Font &font;
    sf::Text text                   = {font,"",0};
    float text_size;
};

#endif //CHIP_8_EMULATOR_BUTTON_H
