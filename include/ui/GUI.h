#ifndef CHIP_8_EMULATOR_GUI_H
#define CHIP_8_EMULATOR_GUI_H

#include <SFML/Graphics.hpp>
#include "Widget.h"
#include "core/CHIP_8.h"

//static std::unordered_map<sf::Keyboard::Scancode, int> key_scancodes{
//        { sf::Keyboard::Scancode::Num1, 0xC},
//        { sf::Keyboard::Scancode::Num2, 0xD},
//        { sf::Keyboard::Scancode::Num3, 0xE},
//        { sf::Keyboard::Scancode::Num4, 0xF},
//        { sf::Keyboard::Scancode::Q,    0x8},
//        { sf::Keyboard::Scancode::W,    0x9},
//        { sf::Keyboard::Scancode::E,    0xA},
//        { sf::Keyboard::Scancode::R,    0xB},
//        { sf::Keyboard::Scancode::A,    0x4},
//        { sf::Keyboard::Scancode::S,    0x5},
//        { sf::Keyboard::Scancode::D,    0x6},
//        { sf::Keyboard::Scancode::F,    0x7},
//        { sf::Keyboard::Scancode::Z,    0x0},
//        { sf::Keyboard::Scancode::X,    0x1},
//        { sf::Keyboard::Scancode::C,    0x2},
//        { sf::Keyboard::Scancode::V,    0x3}
//};

static std::unordered_map<sf::Keyboard::Scancode, int> key_scancodes{
        { sf::Keyboard::Scancode::Num1, 0x0},
        { sf::Keyboard::Scancode::Num2, 0x1},
        { sf::Keyboard::Scancode::Num3, 0x2},
        { sf::Keyboard::Scancode::Num4, 0x3},
        { sf::Keyboard::Scancode::Q,    0x4},
        { sf::Keyboard::Scancode::W,    0x5},
        { sf::Keyboard::Scancode::E,    0x6},
        { sf::Keyboard::Scancode::R,    0x7},
        { sf::Keyboard::Scancode::A,    0x8},
        { sf::Keyboard::Scancode::S,    0x9},
        { sf::Keyboard::Scancode::D,    0xA},
        { sf::Keyboard::Scancode::F,    0xB},
        { sf::Keyboard::Scancode::Z,    0xC},
        { sf::Keyboard::Scancode::X,    0xD},
        { sf::Keyboard::Scancode::C,    0xE},
        { sf::Keyboard::Scancode::V,    0xF}
};

class GUI{
public:

    GUI(sf::RenderWindow *window, CHIP_8 *emul);

    void add(Widget *widget);

    void render();

    void setup_widgets();

    void update(sf::Time delta = {});

    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i& mouse_pos);

    void handle_input(const std::optional<sf::Event> &event);
    void handle_input();
private:
    sf::RenderWindow    *sf_window;
    CHIP_8              *emulator;
    //
    std::vector<Widget*> widgets;
};

#endif //CHIP_8_EMULATOR_GUI_H
