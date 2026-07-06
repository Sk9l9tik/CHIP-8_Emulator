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

int main(){
    CHIP_8 emulator {};
    emulator.load_ROM("6-keypad.ch8");

    sf::RenderWindow window(sf::VideoMode({1280, 960}), "CHIP-8 Emulator");
    GUI gui{&window, &emulator};
    sf::Clock clock;
    float fps = 0.f;
    sf::Font font;
    if(!font.openFromFile("../assets/Galmuri7.ttf")) {
        if (!font.openFromFile("../../assets/Galmuri7.ttf")) {
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
    Button btn_test{"Test", {48, 48}, {0, d_pos.y + 20}, font};

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
            gui.handle_event(event, sf::Mouse::getPosition(window));
        }

        float dt = clock.restart().asSeconds();
        fps = 1.f / dt;
        fps_text.setString(std::to_string(static_cast<int>(fps)));

        gui.handle_input();
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
