#ifndef CHIP_8_EMULATOR_WIDGET_H
#define CHIP_8_EMULATOR_WIDGET_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

struct Padding{
    float up;
    float right;
    float down;
    float left;

    Padding(float u, float r, float d, float l) : up(u), right(r), down(d), left(l) {};
    Padding(float x) : up(x), right(x), down(x), left(x) {};
};

class Widget{
public:
    Padding padding = {0};

    virtual ~Widget() = default;

    virtual void handle_event(const std::optional<sf::Event> &event, [[maybe_unused]] const sf::Vector2i& mouse_pos) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderTarget &target) = 0;

    virtual void set_position(sf::Vector2f _pos)  {pos = _pos;}
    virtual void set_size    (sf::Vector2f _size) {size = _size;}
    virtual void set_parent  (Widget* p)          {parent = p;}

    [[nodiscard]] sf::Vector2f get_position() const {return pos;}
    [[nodiscard]] sf::Vector2f get_size()     const {return size;}

protected:
    Widget() = default;

    Widget *parent = nullptr;

    sf::Vector2f    pos;
    sf::Vector2f    size;
};

#endif //CHIP_8_EMULATOR_WIDGET_H
