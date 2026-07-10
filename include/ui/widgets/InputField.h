#ifndef CHIP_8_EMULATOR_INPUTFIELD_H
#define CHIP_8_EMULATOR_INPUTFIELD_H

#include "ui/Widget.h"

class InputField : public Widget  /*a mojet button...*/{
public:
    InputField(sf::Font font) : text(font, "", 12){

    }

    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i &mouse_pos) override {

    }

    void update() override {

    }

    void render(sf::RenderTarget &target) override {

    }

private:


    std::string input = "";
    sf::Text text;
};

#endif //CHIP_8_EMULATOR_INPUTFIELD_H
