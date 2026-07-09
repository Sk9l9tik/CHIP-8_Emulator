#include "ui/GUI.h"

GUI::GUI(sf::RenderWindow *window, CHIP_8 *emul) {
    sf_window = window;
    emulator = emul;
}

void GUI::handle_input(const std::optional<sf::Event> &event){
    auto &cpu = emulator->get_cpu();

    // Nevajno...
    auto ep = event->getIf<sf::Event::KeyPressed>();
    auto er = event->getIf<sf::Event::KeyReleased>();

    if(!ep && !er) return;

    auto it = key_scancodes.find(ep ? ep->scancode : er->scancode);
    if(it != key_scancodes.end()){
        if(ep) {
            if(!cpu.get_key_state(it->second)){
                cpu.set_key_state(it->second, true);
            }
        }
        else {
            cpu.set_key_state(it->second, false);
        }
    }

    // ...

    if(ep) switch(ep->scancode){
        case sf::Keyboard::Scancode::Escape:
            sf_window->close();
            break;
        default:
            break;
    }

}

void GUI::handle_event(const std::optional<sf::Event> &event, [[maybe_unused]] const sf::Vector2i& mouse_pos) {
    if(const auto& resized = event->getIf<sf::Event::Resized>()){
        sf::FloatRect visible_area({0.f, 0.f}, sf::Vector2f(resized->size));
        sf_window->setView(sf::View(visible_area));
        for(auto& el: widgets){
            el->update();
        }
    }

    if(event->is<sf::Event::KeyPressed>() || event->is<sf::Event::KeyReleased>()){
        handle_input(event);
    }

    for(auto& el : widgets){
        el->handle_event(event, mouse_pos);
    }

}

void GUI::render() {
    for (auto& widget : widgets) {
        widget->render(*sf_window);
    }
}

void GUI::update(sf::Time delta) {
    for(auto& el: widgets){
        el->update();
    }
}

void GUI::add(Widget *widget) {
    widgets.push_back(widget);
    this->setup_widgets();
}

void GUI::setup_widgets() {

}