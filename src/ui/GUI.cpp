#include "ui/GUI.h"


GUI::GUI(sf::RenderWindow &window, CHIP_8 &emul, Debugger &debug) : sf_window(window), emulator(emul), debugger(debug){}

void GUI::handle_input(const std::optional<sf::Event> &event){
    auto &cpu = emulator.get_cpu();

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
            sf_window.close();
            break;
        case sf::Keyboard::Scancode::F5:
            debugger.resume();
            emulator.tick();
            break;
        case sf::Keyboard::Scancode::F9:
            debugger.pause();
            break;
        case sf::Keyboard::Scancode::F10:
            if(debugger.is_paused()){
                debugger.step();
            }
            break;
        case sf::Keyboard::Scancode::Space:
            if(debugger.is_paused()){
                debugger.resume();
                emulator.tick();
            } else {
                debugger.pause();
            }
            break;
        case sf::Keyboard::Scancode::F4:
            debugger.clear_breakpoints();
            break;
        case sf::Keyboard::Scancode::F6:
            debugger.toggle_breakpoint(debugger.get_cpu_state().PC - 0x2);
            break;
        default:
            break;
    }

}

void GUI::handle_event(const std::optional<sf::Event> &event) {
    if(auto _ = event->getIf<sf::Event::Resized>()){
        sf::Vector2u window_size = sf_window.getSize();
        float window_width = static_cast<float>(window_size.x);
        float window_height = static_cast<float>(window_size.y);
        float window_aspect_ratio = window_width / window_height;

        float viewport_x = 0.f;
        float viewport_y =0.f;
        float viewport_width = 1.f;
        float viewport_height = 1.f;

        if (window_aspect_ratio > 1052.f / 960.f) {
            viewport_width = 1052.f / 960.f / window_aspect_ratio;
            viewport_x = (1.f - viewport_width) / 2.f;
        } else {
            viewport_height = window_aspect_ratio / (1052.f / 960.f);
            viewport_y = (1.f - viewport_height) / 2.f;
        }

        auto view = sf_window.getView();
        view.setViewport(sf::FloatRect({viewport_x, viewport_y}, {viewport_width, viewport_height}));
        sf_window.setView(view);

        for(auto& el: widgets){
            el->update();
        }
    }

    if(event->is<sf::Event::KeyPressed>() || event->is<sf::Event::KeyReleased>()){
        handle_input(event);
    }

    for(auto& el : widgets){
        el->handle_event(event);
    }

}

void GUI::render() {
    for (auto& widget : widgets) {
        widget->render(sf_window);
    }
}

void GUI::update() {
    for(auto& el: widgets){
        el->update();
    }
}

void GUI::add(Widget *widget) {
    widgets.push_back(widget);
}

void GUI::setup() {
    if(!ResourceManager::font_set) {
        throw std::runtime_error("Error: set default font.");
    }
}

sf::Vector2f GUI::get_size() {
    float width = 0, height = 0;

    for(auto el : widgets){
        width = std::max(width, el->get_size().x + el->get_position().x);
        height = std::max(height, el->get_size().y + el->get_position().y);
    }

    auto m_size = sf::VideoMode::getDesktopMode().size;

    if(width > static_cast<float>(m_size.x)) width = static_cast<float>(m_size.x);
    if(height > static_cast<float>(m_size.y)) width = static_cast<float>(m_size.y);

    return {width, height};
}
