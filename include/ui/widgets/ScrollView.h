#ifndef CHIP_8_EMULATOR_SCROLLVIEW_H
#define CHIP_8_EMULATOR_SCROLLVIEW_H

#include "ui/Widget.h"

class ScrollView : public Widget{
public:
    ScrollView(sf::Vector2f _size, sf::Vector2f _pos) {
        size = _size; // View size
        pos = _pos;   // Position in window

        view.setSize(size);

        if(!render_target.resize(static_cast<sf::Vector2u>(size)))
            throw std::runtime_error("Error: can't resize scroll view texture.");

        //scrollbar init
        scrollbar_width = 12.f;
        scrollbar_padding = 4.f;

        scrollbar_color = sf::Color(0x969696EE);
        scrollbar_hover_color = sf::Color(0xB4B4B4DD);
        scrollbar_drag_color = sf::Color(0xC8C8C8F0);

        current_scrollbar_color = scrollbar_color;
        is_dragging = false;

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

        // Scroll bar events
        if (auto pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (pressed->button == sf::Mouse::Button::Left) {
                sf::Vector2f mouse_pos = static_cast<sf::Vector2f>(pressed->position);

                if (is_mouse_over_scrollbar(mouse_pos)) {
                    is_dragging = true;
                    drag_offset = mouse_pos.y - get_scrollbar_top();
                    current_scrollbar_color = scrollbar_drag_color;

                    return;
                }

                if (is_mouse_over_scrollbar_track(mouse_pos)) {
                    float track_top = pos.y + scrollbar_padding;
                    float track_height = size.y - scrollbar_padding * 2;
                    float click_ratio = (mouse_pos.y - track_top) / track_height;
                    float max_scroll = std::max(0.f, content_size.y - size.y);
                    scroll = click_ratio * max_scroll;
                    clamp_scroll();

                    return;
                }
            }
        }

        if (auto released = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (released->button == sf::Mouse::Button::Left) {
                if (is_dragging) {
                    is_dragging = false;
                    current_scrollbar_color = is_mouse_over_scrollbar(static_cast<sf::Vector2f>(released->position))
                                              ? scrollbar_hover_color
                                              : scrollbar_color;
                }
            }
        }

        if (auto moved = event->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2f mouse_pos = static_cast<sf::Vector2f>(moved->position);

            if (!is_dragging) {
                if (is_mouse_over_scrollbar(mouse_pos)) {
                    current_scrollbar_color = scrollbar_hover_color;
                } else {
                    current_scrollbar_color = scrollbar_color;
                }
            }

            if (is_dragging) {
                float track_top = pos.y + scrollbar_padding;
                float track_height = size.y - scrollbar_padding * 2;
                float scrollbar_height = get_scrollbar_height();
                float thumb_pos = mouse_pos.y - drag_offset;

                thumb_pos = std::clamp(thumb_pos, track_top, track_top + track_height - scrollbar_height);

                float ratio = (thumb_pos - track_top) / (track_height - scrollbar_height);
                float max_scroll = std::max(0.f, content_size.y - size.y);
                scroll = ratio * max_scroll;
                clamp_scroll();

                sf::Vector2i corrected_pos = static_cast<sf::Vector2i>(mouse_pos - pos);
                corrected_pos.y += static_cast<int>(scroll);
                auto corrected_event = sf::Event(sf::Event::MouseMoved(corrected_pos));
                for (auto widget : widgets) {
                    if (widget != nullptr && is_widget_visible(widget)) {
                        widget->handle_event(corrected_event);
                    }
                }

                return;
            }
        }


        for (auto widget : widgets) {
            if (widget != nullptr && is_widget_visible(widget)) {
                if(auto e = correct_mouse_pos(event)){
                    widget->handle_event(e);
                }
            }
        }
    }

    void update() override {
        for (auto* widget : widgets) {
            if (widget != nullptr && is_widget_visible(widget)) {
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
            })) throw std::runtime_error("ScrollView size can't be 0x0.");
        }

        view.setSize(size);
        view.setCenter({size.x / 2.f, size.y / 2.f + scroll});

        render_target.clear(sf::Color::Transparent);
        render_target.setView(view);

        for (auto* widget : widgets) {
            if (widget != nullptr && is_widget_visible(widget)) {
                widget->render(render_target);
            }
        }

        render_target.display();
        sf::Sprite sprite(render_target.getTexture());
        sprite.setPosition(pos);

        target.draw(sprite);
        render_scrollbar(target);
    }

    void set_size(sf::Vector2f _size) override {
        size = _size;
        view.setSize(_size);
        if(!render_target.resize({
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

    void clear() {
        widgets.clear();

        content_size = {0.f, 0.f};

        scroll = 0.f;

        render_target.clear(sf::Color::Transparent);
        render_target.display();

    }

private:
    bool is_widget_visible(const Widget* widget) const {
        if (widget == nullptr) return false;

        sf::Vector2f widget_pos = widget->get_position();
        sf::Vector2f widget_size = widget->get_size();

        sf::FloatRect widget_rect(widget_pos, widget_size);
        sf::FloatRect visible_rect(
                {0.f, scroll},
                {size.x, scroll + size.y}
        );

        return widget_rect.findIntersection(visible_rect).has_value();
    }

    bool is_mouse_over(sf::Vector2i mouse_pos) const {
        return (sf::FloatRect{pos, size}).contains(static_cast<sf::Vector2f>(mouse_pos));
    }

    bool is_mouse_over(sf::Vector2f mouse_pos) const {
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

    // SCROLLBAR чет он чуть ли не сложнее самого класса вышел
    void render_scrollbar(sf::RenderTarget &target) {
        float max_scroll = std::max(0.f, content_size.y - size.y);

        if (max_scroll <= 0.f) return;

        sf::RectangleShape track;
        track.setPosition({pos.x + size.x - scrollbar_width - scrollbar_padding,
                           pos.y + scrollbar_padding});
        track.setSize({scrollbar_width, size.y - scrollbar_padding * 2});
        track.setFillColor(sf::Color(60, 60, 60, 100));
        track.setOutlineThickness(0.f);
        target.draw(track);

        float scrollbar_height = get_scrollbar_height();
        float thumb_position = get_scrollbar_top();

        sf::RectangleShape thumb;
        thumb.setPosition({pos.x + size.x - scrollbar_width - scrollbar_padding,
                           thumb_position});
        thumb.setSize({scrollbar_width, scrollbar_height});
        thumb.setFillColor(current_scrollbar_color);
        thumb.setOutlineThickness(1.f);
        thumb.setOutlineColor(sf::Color(100, 100, 100, 150));
        target.draw(thumb);
    }

    float get_scrollbar_height() const {
        float max_scroll = std::max(0.f, content_size.y - size.y);
        float visible_ratio = size.y / content_size.y;
        float min_height = 20.f;
        return std::max(min_height, size.y * visible_ratio - scrollbar_padding * 2);
    }

    float get_scrollbar_top() const {
        float max_scroll = std::max(0.f, content_size.y - size.y);
        if (max_scroll == 0.f) return pos.y + scrollbar_padding;

        float track_height = size.y - scrollbar_padding * 2;
        float scrollbar_height = get_scrollbar_height();
        float ratio = scroll / max_scroll;
        return pos.y + scrollbar_padding + ratio * (track_height - scrollbar_height);
    }

    bool is_mouse_over_scrollbar(sf::Vector2f mouse_pos) const {
        float max_scroll = std::max(0.f, content_size.y - size.y);
        if (max_scroll <= 0.f) return false;

        float scrollbar_height = get_scrollbar_height();
        float thumb_position = get_scrollbar_top();

        sf::FloatRect scrollbar_rect(
                {pos.x + size.x - scrollbar_width - scrollbar_padding, thumb_position},
                {scrollbar_width, scrollbar_height}
        );
        return scrollbar_rect.contains(mouse_pos);
    }

    bool is_mouse_over_scrollbar_track(sf::Vector2f mouse_pos) const {
        float max_scroll = std::max(0.f, content_size.y - size.y);
        if (max_scroll <= 0.f) return false;

        sf::FloatRect track_rect(
                {pos.x + size.x - scrollbar_width - scrollbar_padding, pos.y + scrollbar_padding},
                {scrollbar_width, size.y - scrollbar_padding * 2}
        );
        return track_rect.contains(mouse_pos);
    }
    // END SCROLLBAR

    sf::RenderTexture render_target;

    std::vector<Widget*> widgets;
    sf::View view{};

    sf::Vector2f content_size = {0.f,0.f};
    float scroll = 0.f;

    // scrollbar settings

    float scrollbar_width;
    float scrollbar_padding;
    sf::Color scrollbar_color;
    sf::Color scrollbar_hover_color;
    sf::Color scrollbar_drag_color;
    sf::Color current_scrollbar_color;

    bool is_dragging;
    float drag_offset;
};

#endif //CHIP_8_EMULATOR_SCROLLVIEW_H
