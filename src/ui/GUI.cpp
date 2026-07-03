//
// Created by V on 01.07.2026.
//

#include "ui/GUI.h"

GUI::GUI(sf::RenderWindow *window) {
    sf_window = window;


}

void GUI::handle_event(const std::optional<sf::Event> &event) {
    if(const auto& resized = event->getIf<sf::Event::Resized>()){
        sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
        sf_window->setView(sf::View(visibleArea));
        for(auto& el: widgets){
            el->update();
        }
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