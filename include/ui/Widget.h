#ifndef CHIP_8_EMULATOR_WIDGET_H
#define CHIP_8_EMULATOR_WIDGET_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Style.h"
#include "ResourceManager.h"

class Widget{
public:
    Style style;

    virtual ~Widget() = default;

    virtual void handle_event(const std::optional<sf::Event> &event) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderTarget &target) = 0;

    virtual void set_position(sf::Vector2f _pos)  {pos = _pos;}
    virtual void set_size    (sf::Vector2f _size) {size = _size;}
    virtual void set_parent  (Widget* p)          {parent = p;}

    [[nodiscard]] virtual sf::Vector2f get_position() const {return pos;}
    [[nodiscard]] virtual sf::Vector2f get_size()     const {return size;}

protected:
    Widget *parent = nullptr;

    sf::Vector2f    pos;  // Global
    sf::Vector2f    size; // Global
};

#endif //CHIP_8_EMULATOR_WIDGET_H
