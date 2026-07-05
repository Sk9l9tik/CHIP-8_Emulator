#ifndef CHIP_8_EMULATOR_WIDGET_H
#define CHIP_8_EMULATOR_WIDGET_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Widget{
public:
    //

    virtual ~Widget() = default;

    virtual void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i& mouse_pos) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderTarget &target) = 0;

    virtual void set_position(sf::Vector2f _pos)  {pos = _pos;}
    virtual void set_size    (sf::Vector2f _size) {size = _size;}

    [[nodiscard]] sf::Vector2f get_position() const {return pos;}
    [[nodiscard]] sf::Vector2f get_size()     const {return size;}
    //

private:
    sf::Vector2f    size;
    sf::Vector2f    pos;

    Widget *parent = nullptr; // 0_o o_0
};

#endif //CHIP_8_EMULATOR_WIDGET_H
