//
// Created by V on 01.07.2026.
//
#include "ui/GUI.h"

GUI::GUI(sf::RenderWindow *window, CHIP_8 *emul) {
    sf_window = window;
    emulator = emul;
}

[[maybe_unused]] void GUI::handle_input(const std::optional<sf::Event> &event){
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
                //printf("%x pressed\n", it->second);
            }
        }
        else {
            cpu.set_key_state(it->second, false);
            //printf("%x released\n", it->second);
        }
    }
}

void GUI::handle_input(){
    auto &cpu = emulator->get_cpu();

    for(auto& key : key_scancodes){
       cpu.set_key_state(key.second, sf::Keyboard::isKeyPressed(key.first));
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)){
        sf_window->close();
    }
}

void GUI::handle_event(const std::optional<sf::Event> &event, const sf::Vector2i& mouse_pos) {
    if(const auto& resized = event->getIf<sf::Event::Resized>()){
        sf::FloatRect visible_area({0.f, 0.f}, sf::Vector2f(resized->size));
        sf_window->setView(sf::View(visible_area));
        for(auto& el: widgets){
            el->update();
        }
    }

    if(event->is<sf::Event::MouseButtonPressed>()){
        for(auto& el : widgets){
            el->handle_event(event, mouse_pos);
        }
    }

//    if(event->is<sf::Event::KeyPressed>() || event->is<sf::Event::KeyReleased>()){
//        handle_input(event);
//    }

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