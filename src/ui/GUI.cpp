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
    if(auto resized = event->getIf<sf::Event::Resized>()){
        sf::Vector2u windowSize = sf_window.getSize();
        float windowWidth = static_cast<float>(windowSize.x);
        float windowHeight = static_cast<float>(windowSize.y);
        float windowAspectRatio = windowWidth / windowHeight;

        float viewportX = 0.f;
        float viewportY = 0.f;
        float viewportWidth = 1.f;
        float viewportHeight = 1.f;

        if (windowAspectRatio > 1052.f/960.f) {
            viewportWidth = 1052.f/960.f / windowAspectRatio;
            viewportX = (1.f - viewportWidth) / 2.f;
        } else {
            viewportHeight = windowAspectRatio / (1052.f/ 960.f);
            viewportY = (1.f - viewportHeight) / 2.f;
        }

        auto view = sf_window.getView();
        view.setViewport(sf::FloatRect({viewportX, viewportY}, {viewportWidth, viewportHeight}));
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