#ifndef CHIP_8_EMULATOR_BUTTON_H
#define CHIP_8_EMULATOR_BUTTON_H

#include "ui/Widget.h"

class Button : public Widget{
public:
    Button(const std::string& btn_text, sf::Vector2f size, sf::Vector2f pos, sf::Font &btn_font) : font(btn_font){
        text_size = 16;
        text = sf::Text(font, btn_text, text_size);
        bounds = sf::FloatRect(pos, size);

        bg_texture.resize({static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)}); // TODO: catch [[nodiscard]]

        bg_texture.clear(sf::Color::Transparent);

        sf::RectangleShape rect(size);
        rect.setFillColor(sf::Color::White);
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(0);


        bg_texture.draw(rect);
        bg_texture.draw(text);
        bg_texture.display();

        background.setTexture(bg_texture.getTexture(), true);
        background.setPosition(pos);
        background.setScale({1,1});

        text.setFillColor(sf::Color::Black);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});

        sf::FloatRect spriteBounds = background.getGlobalBounds();
        text.setPosition({spriteBounds.position.x + spriteBounds.size.x / 2.f,
                          spriteBounds.position.y + spriteBounds.size.y / 2.f - 2});

        state = State::Normal;
    }

    ~Button() override = default;

    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i &mouse_pos) override {
        if(const auto& ev = event->getIf<sf::Event::MouseButtonPressed>()){
            if(ev->button == sf::Mouse::Button::Left && bounds.contains(static_cast<sf::Vector2f>(ev->position))){
                state = State::Pressed;

                printf("btn clicked\n");
            }
        }
        if(const auto& ev = event->getIf<sf::Event::MouseMoved>()){
            if(bounds.contains(static_cast<sf::Vector2f>(ev->position))){
                state = State::Hovered;
                printf("btn hovered\n");
            }
        }
    }

    void update() override {

    }

    void render(sf::RenderTarget &target) override {
        target.draw(background);
        target.draw(text);
    }

    void set_position(sf::Vector2f _pos) override {
        pos = _pos;
        bounds.position = _pos;
    }

    void set_size(sf::Vector2f _size) override {
        size = _size;
        bounds.size = _size;
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

    sf::FloatRect bounds;

    std::unordered_map<std::string, sf::Image> textures;

    sf::RenderTexture bg_texture    = sf::RenderTexture{};
    sf::Sprite background           = sf::Sprite{bg_texture.getTexture()};
    sf::Font &font;
    sf::Text text                   = {font,"",0};
    uint32_t text_size;
};

#endif //CHIP_8_EMULATOR_BUTTON_H
