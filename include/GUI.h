#ifndef CHIP_8_EMULATOR_GUI_H
#define CHIP_8_EMULATOR_GUI_H

#include <SFML/Graphics.hpp>
#include "Widget.h"

class GUI{
public:

    GUI(sf::RenderWindow &window, std::string title);

    void add(Widget &widget);

    void render();

    void setup_widgets();

    void update();

    void handle_events();

private:
    sf::RenderWindow    *sf_window;
    std::string         title;

    //flags

    bool resizing = false;
    bool dragging = false;

    //
};

#endif //CHIP_8_EMULATOR_GUI_H
