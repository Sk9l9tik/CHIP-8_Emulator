#ifndef CHIP_8_EMULATOR_GUI_H
#define CHIP_8_EMULATOR_GUI_H

#include <SFML/Graphics.hpp>
#include "Widget.h"



class GUI{
public:

    GUI(sf::RenderWindow *window);

    void add(Widget *widget);

    void render();

    void setup_widgets();

    void update(sf::Time delta = {});

    void handle_event(const std::optional<sf::Event> &event);

private:
    sf::RenderWindow    *sf_window;

    //
    std::vector<Widget*> widgets;
};

#endif //CHIP_8_EMULATOR_GUI_H
