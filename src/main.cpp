#include <ios>
#include <iostream>

#include "core/CHIP_8.h"
#include "core/Memory.h"

#include "ui/GUI.h"

#include "ui/widgets/Button.h"
#include "ui/widgets/Display.h"

#define CHIP8_DEBUG1

#ifdef CHIP8_DEBUG
#include "debug/Debugger.h"
#endif

// Пишет в консоль вообще по каждому SFML событию, можете в отдельный файл как то выделить типа
// tools или misc или helpers или utils или че там пишут я забыл уже но мне лично лень просто
void SAERMO_logger(const std::optional<sf::Event>& event)
{
    // Проверка, что событие существует
    if (!event.has_value())
    {
        std::cout << "Event: nullopt" << std::endl;
        return;
    }

    // Закрытие окна
    if (event->is<sf::Event::Closed>())
    {
        std::cout << "Event: Closed" << std::endl;
    }
        // Изменение размера
    else if (const auto* resized = event->getIf<sf::Event::Resized>())
    {
        std::cout << "Event: Resized (width=" << resized->size.x
                  << ", height=" << resized->size.y << ")" << std::endl;
    }
        // Потеря фокуса
    else if (event->is<sf::Event::FocusLost>())  // было event.is -> event->is
    {
        std::cout << "Event: FocusLost" << std::endl;
    }
        // Получение фокуса
    else if (event->is<sf::Event::FocusGained>())  // было event.is -> event->is
    {
        std::cout << "Event: FocusGained" << std::endl;
    }

        // === Клавиатура ===
    else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
    {
        std::cout << "Event: KeyPressed (code=" << static_cast<int>(keyPressed->code)
                  << ", scancode=" << static_cast<int>(keyPressed->scancode)
                  << ", alt=" << keyPressed->alt
                  << ", control=" << keyPressed->control
                  << ", shift=" << keyPressed->shift
                  << ", system=" << keyPressed->system << ")" << std::endl;
    }
    else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
    {
        std::cout << "Event: KeyReleased (code=" << static_cast<int>(keyReleased->code)
                  << ", scancode=" << static_cast<int>(keyReleased->scancode)
                  << ", alt=" << keyReleased->alt
                  << ", control=" << keyReleased->control
                  << ", shift=" << keyReleased->shift
                  << ", system=" << keyReleased->system << ")" << std::endl;
    }
    else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
    {
        std::cout << "Event: TextEntered (unicode=" << static_cast<int>(textEntered->unicode)
                  << ", char='" << static_cast<char>(textEntered->unicode) << "')" << std::endl;
    }

        // === Мышь ===
//    else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
//    {
//        std::cout << "Event: MouseMoved (x=" << mouseMoved->position.x
//                  << ", y=" << mouseMoved->position.y << ")" << std::endl;
//    }
//    else if (const auto* mouseMovedRaw = event->getIf<sf::Event::MouseMovedRaw>())
//    {
//        std::cout << "Event: MouseMovedRaw (dx=" << mouseMovedRaw->delta.x
//                  << ", dy=" << mouseMovedRaw->delta.y << ")" << std::endl;
//    }
    else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
    {
        std::cout << "Event: MouseButtonPressed (button=" << static_cast<int>(mousePressed->button)
                  << ", x=" << mousePressed->position.x
                  << ", y=" << mousePressed->position.y << ")" << std::endl;
    }
    else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
    {
        std::cout << "Event: MouseButtonReleased (button=" << static_cast<int>(mouseReleased->button)
                  << ", x=" << mouseReleased->position.x
                  << ", y=" << mouseReleased->position.y << ")" << std::endl;
    }
    else if (const auto* wheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>())
    {
        std::cout << "Event: MouseWheelScrolled (wheel=" << static_cast<int>(wheelScrolled->wheel)
                  << ", delta=" << wheelScrolled->delta
                  << ", x=" << wheelScrolled->position.x
                  << ", y=" << wheelScrolled->position.y << ")" << std::endl;
    }
    else if (event->is<sf::Event::MouseEntered>())  // было event.is -> event->is
    {
        std::cout << "Event: MouseEntered" << std::endl;
    }
    else if (event->is<sf::Event::MouseLeft>())  // было event.is -> event->is
    {
        std::cout << "Event: MouseLeft" << std::endl;
    }

        // === Джойстик ===
    else if (event->is<sf::Event::JoystickConnected>())  // было event.is -> event->is
    {
        std::cout << "Event: JoystickConnected (id="
                  << event->getIf<sf::Event::JoystickConnected>()->joystickId << ")" << std::endl;
    }
    else if (event->is<sf::Event::JoystickDisconnected>())  // было event.is -> event->is
    {
        std::cout << "Event: JoystickDisconnected (id="
                  << event->getIf<sf::Event::JoystickDisconnected>()->joystickId << ")" << std::endl;
    }
    else if (const auto* joyMoved = event->getIf<sf::Event::JoystickMoved>())
    {
        std::cout << "Event: JoystickMoved (id=" << joyMoved->joystickId
                  << ", axis=" << static_cast<int>(joyMoved->axis)
                  << ", position=" << joyMoved->position << ")" << std::endl;
    }
    else if (const auto* joyPressed = event->getIf<sf::Event::JoystickButtonPressed>())
    {
        std::cout << "Event: JoystickButtonPressed (id=" << joyPressed->joystickId
                  << ", button=" << joyPressed->button << ")" << std::endl;
    }
    else if (const auto* joyReleased = event->getIf<sf::Event::JoystickButtonReleased>())
    {
        std::cout << "Event: JoystickButtonReleased (id=" << joyReleased->joystickId
                  << ", button=" << joyReleased->button << ")" << std::endl;
    }

        // === Сенсорный ввод ===
    else if (const auto* touchBegan = event->getIf<sf::Event::TouchBegan>())
    {
        std::cout << "Event: TouchBegan (finger=" << touchBegan->finger
                  << ", x=" << touchBegan->position.x
                  << ", y=" << touchBegan->position.y << ")" << std::endl;
    }
    else if (const auto* touchMoved = event->getIf<sf::Event::TouchMoved>())
    {
        std::cout << "Event: TouchMoved (finger=" << touchMoved->finger
                  << ", x=" << touchMoved->position.x
                  << ", y=" << touchMoved->position.y << ")" << std::endl;
    }
    else if (const auto* touchEnded = event->getIf<sf::Event::TouchEnded>())
    {
        std::cout << "Event: TouchEnded (finger=" << touchEnded->finger
                  << ", x=" << touchEnded->position.x
                  << ", y=" << touchEnded->position.y << ")" << std::endl;
    }
}

int main(){
    CHIP_8 emulator {};
    emulator.load_ROM("6-keypad.ch8");

    sf::RenderWindow window(sf::VideoMode({640, 640}), "CHIP-8 Emulator");
    GUI gui{&window, &emulator};
    sf::Clock clock;
    float fps = 0.f;
    sf::Font font;
    if(!font.openFromFile("../assets/fonts/Galmuri7.ttf")) {
        if (!font.openFromFile("../../assets/fonts/Galmuri7.ttf")) {
            return -1;
        };
    }

    sf::Text fps_text(font, std::to_string(300), 16);
    fps_text.setFillColor(sf::Color::Black);
    fps_text.setPosition({static_cast<float>(window.getSize().x)
                             - fps_text.getGlobalBounds().size.x, 0.f});


    Display display{emulator.get_frame_buffer()};
    display.set_size({640, 320});

    gui.add(&display);


    auto d_pos = display.get_size();
    Button btn_test{"0", {128, 128}, {0, d_pos.y + 20}, font};

    btn_test.load_texture("../assets/sprites/KBButton_Default.png", Button::State::Normal);
    btn_test.load_texture("../assets/sprites/KBButton_Hovered.png", Button::State::Hovered);
    btn_test.load_texture("../assets/sprites/KBButton_Locked.png", Button::State::Locked);
    btn_test.load_texture("../assets/sprites/KBButton_Pressed.png", Button::State::Pressed);
    // ниче лучше не придумал
    btn_test.set_on_click([&emulator](){
        emulator.get_cpu().set_key_state(0x0, true);
    });
    btn_test.set_on_release([&emulator](){
        emulator.get_cpu().set_key_state(0x0, false);
    });
    gui.add(&btn_test);
#ifdef CHIP8_DEBUG
    Debugger d(emulator);

    auto disassembled = d.disassemble(Memory::ROM_START, 20);
    std::cout << "addr\topcode\tmnemonic\n";
    // for(auto&& [addr, opc, mnem] : disassembled) {
    //   std::cout << "0x" << std::hex << std::uppercase << addr << "\t" << "0x" << opc << "\t" << mnem << '\n';
    // }  

    auto a = d.memory_dump(Memory::ROM_START, Memory::MEM_SIZE);

    std::cout << a << '\n';  
    d.pause(); // cause no-bp is not set
    // d.set_breakpoint(0x20a);
    // std::cout << "Paused at PC = 0x" << std::hex << emulator.get_cpu().get_PC() << "\n";
#endif

    // \/\/\/
    window.requestFocus();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
#ifdef CHIP8_DEBUG
            if(const auto* kp = event->getIf<sf::Event::KeyPressed>()){
                if(kp->scancode == sf::Keyboard::Scancode::F5){
                    d.resume();
                    std::cout << "Resumed\n";
                }
                else if(kp->scancode == sf::Keyboard::Scancode::F9){
                    d.pause();
                    std::cout << "Paused\n";
                }
                else if(kp->scancode == sf::Keyboard::Scancode::F10 && d.is_paused()){
                    d.step();
                    std::cout << "Step -> PC = 0x" << std::hex
                               << emulator.get_cpu().get_PC() << "\n";
                }
                else if(kp->scancode == sf::Keyboard::Scancode::F6){
                    uint16_t pc = emulator.get_cpu().get_PC();
                    d.toggle_breakpoint(pc);
                    std::cout << (d.has_breakpoint(pc) ? "Breakpoint set at 0x"
                                                        : "Breakpoint cleared at 0x")
                               << std::hex << pc << "\n";
                }
            }
#endif
            SAERMO_logger(event);
            gui.handle_event(event, sf::Mouse::getPosition(window));
        }

        float dt = clock.restart().asSeconds();
        fps = 1.f / dt;
        fps_text.setString(std::to_string(static_cast<int>(fps)));


#ifdef CHIP8_DEBUG
        d.update();
        if(d.is_paused() == false || true){}
#else
        emulator.tick();
#endif
        window.clear(sf::Color::Black);
        gui.update();
        gui.render();
        window.draw(fps_text);
        window.display();
    }

    return 0;
}
