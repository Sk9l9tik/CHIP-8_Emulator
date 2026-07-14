#ifndef CHIP_8_EMULATOR_LABEL_H
#define CHIP_8_EMULATOR_LABEL_H

#include <functional>
#include <utility>
#include "ui/Widget.h"
#include "utils.hpp"

class Label : public Widget {
public:
    Label(const std::string &_text, sf::Vector2f _size, sf::Vector2f _pos) : background({_size}){
        pos = _pos;
        size = _size;

        text.setString(_text);

        background.setSize(size);
        background.setPosition(pos);
        background.setFillColor(sf::Color::Transparent);
        pos += {style.padding.left, style.padding.top};

        padding_bg.setSize(size);
        padding_bg.setPosition(pos);
        padding_bg.setFillColor(sf::Color::Transparent);

        padding_bg.setPosition(
                background.getPosition() -
                sf::Vector2f{style.padding.left, style.padding.top}
        );



        utils::center_text(text, background.getGlobalBounds());
    }

    Label() : Label("", {0.f,0.f}, {0.f,0.f}) {}

    void update() override {
        on_update();

    }

    void handle_event(const std::optional<sf::Event> &event) override {
        //
    }

    void render(sf::RenderTarget &target) override {
        target.draw(padding_bg);
        target.draw(background);
        target.draw(text);
    }

    void set_size(sf::Vector2f _size) override {
        size = _size;
        background.setSize(_size);
        padding_bg.setSize(
                background.getSize()+
                sf::Vector2f{style.padding.left + style.padding.right, style.padding.top+style.padding.bottom}
        );

        auto_resize_bg(bg_size_auto);
    }

    void set_position(sf::Vector2f _pos) override {
        pos = _pos;
        pos += {style.padding.left, style.padding.top};
        background.setPosition(pos);
        padding_bg.setPosition(
                background.getPosition() -
                sf::Vector2f{style.padding.left, style.padding.top}
            );

        auto_resize_bg(bg_size_auto);
    }

    void set_bg_color(uint32_t color) {
        background.setFillColor(static_cast<sf::Color>(color));
        padding_bg.setFillColor(static_cast<sf::Color>(color));
    }

    void set_bg_color(const sf::Color& color) {
        background.setFillColor(color);
        padding_bg.setFillColor(color);
    }

    void set_char_size(uint32_t new_size){
        text.setCharacterSize(new_size);
        auto_resize_bg(bg_size_auto);
    }

    void set_string(const std::string &new_text){
        text.setString(new_text);

        auto_resize_bg(bg_size_auto);
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
            padding_bg.setSize(
                    background.getSize()+
                    sf::Vector2f{style.padding.left + style.padding.right, style.padding.top+style.padding.bottom}
                );

            size = text.getGlobalBounds().size;
        } else {
            background.setSize(size);
            background.setPosition(pos);
            padding_bg.setSize(
                    background.getSize()+
                    sf::Vector2f{style.padding.left + style.padding.right, style.padding.top+style.padding.bottom}
            );
            switch(origin){
                case Origin::Center:
                    utils::center_text(text, background.getGlobalBounds());
                    break;
                case Origin::Left:
                    utils::left_text(text, background.getGlobalBounds());
                    break;
                case Origin::Right:
                    utils::right_text(text, background.getGlobalBounds());
                    break;
            }

        }
    }

    void set_on_update(std::function<void()> func){
        on_update = std::move(func);
    }

    void stick_to_left(){
        origin = Origin::Left;
        auto_resize_bg(bg_size_auto);
    }

    void stick_to_center(){
        origin = Origin::Center;
        auto_resize_bg(bg_size_auto);
    }

    void stick_to_right(){
        origin = Origin::Right;
        auto_resize_bg(bg_size_auto);
    }

    sf::Vector2f get_size() const override {
        return size + sf::Vector2f{style.padding.left + style.padding.right, style.padding.top + style.padding.bottom};
    }
private:
    enum class Origin{ // Works only without auto background size
        Left,
        Center,
        Right
    } origin = Origin::Center;

    bool bg_size_auto = false;

    sf::Text text{ResourceManager::get_font(), "", 24};
    sf::RectangleShape background;
    sf::RectangleShape padding_bg;

    std::function<void()> on_update = [](){};
};

#endif //CHIP_8_EMULATOR_LABEL_H
