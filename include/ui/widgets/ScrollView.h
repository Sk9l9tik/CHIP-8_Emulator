#ifndef CHIP_8_EMULATOR_SCROLLVIEW_H
#define CHIP_8_EMULATOR_SCROLLVIEW_H

#include "ui/Widget.h"

// Пока чисто вертикальный, да и вряд ли другой понадобится
class ScrollView : public Widget{
public:
    ScrollView(sf::Vector2f _size, sf::Vector2f _pos) {
        size = _size; // View size
        pos = _pos;   // In window

        view.setSize(_size);

        if(!render_target.resize(static_cast<sf::Vector2u>(_size)))
            throw std::runtime_error("Error: can't resize scroll view texture.");
    }

    bool add_widget(Widget* widget) {
        if(!widget) return false;

        widgets.push_back(widget);
        update_content_size();
        clamp_scroll();

        return true;
    }

    void handle_event(const std::optional<sf::Event> &event) override {
        if (event->is<sf::Event::MouseWheelScrolled>()) {
            auto wheel = event->getIf<sf::Event::MouseWheelScrolled>();

            if (wheel && is_mouse_over(wheel->position))
            {
                scroll -= wheel->delta * 20.f;
                clamp_scroll();
            }
        }

        for (auto widget : widgets) {
            if (widget != nullptr) {
                if(auto e = correct_mouse_pos(event)){
                    widget->handle_event(e);
                }
            }
        }

        if(auto e = event->getIf<sf::Event::MouseMoved>()){
            if(!is_mouse_over(e->position)){
                for(auto widget : widgets){
                    if(auto* btn = dynamic_cast<Button*>(widget)){
                        btn->set_state(btn->is_locked() ? Button::State::Locked : Button::State::Normal);
                    }
                }
            }
        }
    }

    void update() override {
        for (auto* widget : widgets) {
            if (widget != nullptr) {
                widget->update();
            }
        }
    }

    void render(sf::RenderTarget &target) override {
        if (widgets.empty()) return;

        if (render_target.getSize() != sf::Vector2u(size.x, size.y)) {
            if (!render_target.resize({
                static_cast<unsigned int>(size.x),
                static_cast<unsigned int>(size.y)
            })) throw std::runtime_error("ScrollView size is (0,0), otshlefuyte, please.");
        }

        view.setSize(size);
        view.setCenter({size.x / 2.f, size.y / 2.f + scroll});

        render_target.clear(sf::Color::Transparent);
        render_target.setView(view);

        for (auto* widget : widgets) {
            if (widget != nullptr) {
                widget->render(render_target);
            }
        }

        render_target.display();
        sf::Sprite sprite(render_target.getTexture());
        sprite.setPosition(pos);

        target.draw(sprite);
    }

    void set_size(sf::Vector2f _size) override {
        size = _size;
        view.setSize(_size);
        if(render_target.resize({
            static_cast<uint32_t>(_size.x),
            static_cast<uint32_t>(_size.y)}
        )){
            throw std::runtime_error("Error: can't resize view texture");
        }
        clamp_scroll();
    }

    void set_scroll(float _scroll) {
        scroll = _scroll;
        clamp_scroll();
    }

    float get_scroll() const { return scroll; }

private:
    bool is_mouse_over(sf::Vector2i mouse_pos){
        return (sf::FloatRect{pos, size}).contains(static_cast<sf::Vector2f>(mouse_pos));
    }

    bool is_mouse_over(sf::Vector2f mouse_pos){
        return (sf::FloatRect{pos, size}).contains(mouse_pos);
    }

    void update_content_size() {
        float max_height = 0.f;
        float max_width = 0.f;

        for (auto* widget : widgets) {
            if (widget != nullptr) {
                sf::Vector2f widget_pos  = widget->get_position();
                sf::Vector2f widget_size = widget->get_size();

                max_width  = std::max(max_width, widget_pos.x + widget_size.x);
                max_height = std::max(max_height, widget_pos.y + widget_size.y);
            }
        }

        content_size = {max_width, max_height};
    }

    void clamp_scroll() {
        float max_scroll = std::max(0.f, content_size.y - size.y);
        scroll = std::clamp(scroll, 0.f, max_scroll);
    }

    std::optional<sf::Event> correct_mouse_pos(const std::optional<sf::Event> &event){
        if (!event.has_value()) return std::nullopt;

        if(auto e = event->getIf<sf::Event::MouseMoved>()){
            if(!is_mouse_over(static_cast<sf::Vector2f>(e->position))) return std::nullopt;
            sf::Vector2f mouse_pos_corrected = static_cast<sf::Vector2f>(e->position) - pos;
            mouse_pos_corrected.y += scroll;
            return sf::Event(sf::Event::MouseMoved(static_cast<sf::Vector2i>(mouse_pos_corrected)));
        }
        if(auto e = event->getIf<sf::Event::MouseButtonPressed>()){
            if(!is_mouse_over(static_cast<sf::Vector2f>(e->position))) return std::nullopt;
            sf::Vector2f mouse_pos_corrected = static_cast<sf::Vector2f>(e->position) - pos;
            mouse_pos_corrected.y += scroll;
            return sf::Event(sf::Event::MouseButtonPressed(
                    e->button,
                    static_cast<sf::Vector2i>(mouse_pos_corrected)
                    ));
        }
        if(auto e = event->getIf<sf::Event::MouseButtonReleased>()){
            if(!is_mouse_over(static_cast<sf::Vector2f>(e->position))) return std::nullopt;
            sf::Vector2f mouse_pos_corrected = static_cast<sf::Vector2f>(e->position) - pos;
            mouse_pos_corrected.y += scroll;
            return sf::Event(sf::Event::MouseButtonReleased(
                    e->button,
                    static_cast<sf::Vector2i>(mouse_pos_corrected)
            ));
        }
        if(auto e = event->getIf<sf::Event::MouseWheelScrolled>()){
            if(!is_mouse_over(static_cast<sf::Vector2f>(e->position))) return std::nullopt;
            sf::Vector2f mouse_pos_corrected = static_cast<sf::Vector2f>(e->position) - pos;
            mouse_pos_corrected.y += scroll;
            return sf::Event(sf::Event::MouseWheelScrolled(
                    e->wheel,
                    e->delta,
                    static_cast<sf::Vector2i>(mouse_pos_corrected)
            ));
        }

        return event;
    }

    sf::RenderTexture render_target;

    std::vector<Widget*> widgets;
    sf::View view{};

    sf::Vector2f content_size = {0.f,0.f};
    float scroll = 0.f;
};

#endif //CHIP_8_EMULATOR_SCROLLVIEW_H
