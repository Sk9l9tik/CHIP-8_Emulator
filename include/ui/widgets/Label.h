#ifndef CHIP_8_EMULATOR_LABEL_H
#define CHIP_8_EMULATOR_LABEL_H

#include "ui/Widget.h"
#include "utils.hpp"

using utils::center_text;

class Label : public Widget {
public:
    Label(const std::string &_text, sf::Vector2f _size, sf::Vector2f _pos) : background({_size}){
        pos = _pos;
        size = _size;

        text.setString(_text);

        background.setSize(_size);
        background.setPosition(_pos);
        background.setFillColor(sf::Color::Transparent);

        center_text(text, background.getGlobalBounds());
    }

    void update() override {

    }

    void handle_event(const std::optional<sf::Event> &event) override {
        //
    }

    void render(sf::RenderTarget &target) override {
        target.draw(background);
        target.draw(text);
    }

    void set_size(sf::Vector2f _size) override {
        size = _size;
        background.setSize(_size);

        if(!bg_size_auto) center_text(text, background.getGlobalBounds());
    }

    void set_position(sf::Vector2f _pos) override {
        pos = _pos;
        background.setPosition(_pos);
        if(bg_size_auto) {
            text.setOrigin({0,0});
            text.setPosition(pos);
            background.setSize(text.getLocalBounds().size);
            background.setPosition(text.getLocalBounds().position);
        } else {
            background.setSize(size);
            background.setPosition(pos);
            center_text(text, background.getGlobalBounds());
        }
    }

    void set_bg_color(uint32_t color) {
        background.setFillColor(static_cast<sf::Color>(color));
    }

    void set_bg_color(const sf::Color& color) {
        background.setFillColor(color);
    }

    void set_char_size(uint32_t new_size){
        text.setCharacterSize(new_size);
        if(!bg_size_auto) center_text(text, background.getGlobalBounds());
    }

    void set_string(const std::string &new_text){
        text.setString(new_text);

        if(bg_size_auto) {
            text.setOrigin({0,0});
            text.setPosition(pos);
            background.setSize(text.getLocalBounds().size);
            background.setPosition(text.getLocalBounds().position);
        } else
            center_text(text, background.getGlobalBounds());
    }

    void set_text_color(uint32_t color) {
        text.setFillColor(static_cast<sf::Color>(color));
    }

    void set_text_color(const sf::Color& color) {
        text.setFillColor(color);
    }

    void auto_resize_bg(bool auto_resize){
        bg_size_auto = auto_resize;
        if(bg_size_auto) {
            text.setOrigin({0,0});
            text.setPosition(pos);
            background.setSize(text.getGlobalBounds().size);
            background.setPosition(text.getGlobalBounds().position);
        } else {
            background.setSize(size);
            background.setPosition(pos);
            center_text(text, background.getGlobalBounds());
        }
    }
private:
    bool bg_size_auto = true;

    sf::Text text{ResourceManager::get_font(), "", 24};
    sf::RectangleShape background;
};

#endif //CHIP_8_EMULATOR_LABEL_H
