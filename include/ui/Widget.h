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

    [[nodiscard]] virtual sf::Vector2f get_position() const {
        return {pos.x + style.margin.left, pos.y + style.margin.top};
    }

    [[nodiscard]] virtual sf::Vector2f get_size()     const {
        return {size.x + style.padding.left + style.padding.right,
                size.y + style.padding.top + style.padding.bottom};
    }

protected:
    Widget *parent = nullptr;

    sf::Vector2f    pos;
    sf::Vector2f    size;
};

#endif //CHIP_8_EMULATOR_WIDGET_H
