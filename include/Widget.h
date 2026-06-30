#ifndef CHIP_8_EMULATOR_WIDGET_H
#define CHIP_8_EMULATOR_WIDGET_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Widget{
public:
    int             z_index;
    sf::Vector2f    size;
    sf::Vector2f    pos;

    //

    virtual ~Widget() = default;

    virtual void handle_event(const sf::Event& event, const sf::Vector2f& mouse_pos) = 0;
    virtual void update() = 0;
    virtual bool render(sf::RenderTarget &target) = 0;

    virtual void set_position(sf::Vector2f pos) = 0;
    virtual void set_size(sf::Vector2f pos) = 0;

    //
};

#endif //CHIP_8_EMULATOR_WIDGET_H
