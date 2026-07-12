#ifndef CHIP_8_EMULATOR_TOGGLE_H
#define CHIP_8_EMULATOR_TOGGLE_H

#include "ui/widgets/Button.h"

class Toggle : public Button{
public:
    using Button::Button;

    void handle_event(const std::optional<sf::Event> &event) override {
        // Lock check
        if(is_locked()) return;

        // Hover check
        if(const auto& ev = event->getIf<sf::Event::MouseMoved>()){
            const bool hover = bounds.contains(static_cast<sf::Vector2f>(ev->position));
            if( hover && state != State::Hovered && state != State::Pressed){
                state = State::Hovered;
                on_mouse_entered();
            } else if(!hover && state == State::Hovered) {
                state = State::Normal;
                on_mouse_exited();
            }
        }

        //Is pressed
        if(const auto& ev = event->getIf<sf::Event::MouseButtonPressed>()){
            if(state != State::Pressed && ev->button == sf::Mouse::Button::Left
               && bounds.contains(static_cast<sf::Vector2f>(ev->position)))
            {
                state = State::Pressed;
                on_click();
                return; // To avoid auto-release
            }
        }

        //Is released
        if(const auto& ev = event->getIf<sf::Event::MouseButtonPressed>()){
            if(state == State::Pressed && ev->button == sf::Mouse::Button::Left
               && bounds.contains(static_cast<sf::Vector2f>(ev->position)))
            {
                state = bounds.contains(static_cast<sf::Vector2f>(ev->position)) ? State::Hovered : State::Normal;
                on_release();
            }
        }
    }
};

#endif //CHIP_8_EMULATOR_TOGGLE_H
