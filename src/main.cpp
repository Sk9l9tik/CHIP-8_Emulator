#include <cstdio>

#include <iostream>

#include "core/CHIP_8.h"

#include "ui/GUI.h"
#include "ui/widgets/Display.h"

#define CHIP8_DEBUG1

#ifdef CHIP8_DEBUG
#include "debug/Debugger.h"
#endif

int main(){
    CHIP_8 emulator {};
    emulator.load_ROM("6-keypad.ch8"); //укажите здесь пути до этого прикола свои

    sf::RenderWindow window(sf::VideoMode({640, 320}), "CHIP-8 Emulator");
    GUI gui{&window, &emulator};


    //
    sf::Clock clock;
    float fps = 0.f;
    sf::Font font;
    if(!font.openFromFile("../assets/Galmuri7.ttf")) return -1;

    sf::Text fps_text(font, std::to_string(300), 16);
    fps_text.setFillColor(sf::Color::Black);

    fps_text.setPosition({static_cast<float>(window.getSize().x)
                             - fps_text.getGlobalBounds().size.x, 0.f});
    //
    Display display{emulator.get_frame_buffer()};
    display.set_size({640, 320});



    gui.add(&display);
#ifdef CHIP8_DEBUG
    Debugger d(emulator);
    
    d.set_breakpoint(0x200);

    while(!d.is_paused())
      d.update();
    std::cout << "Hit breakpoint at PC = 0x" << std::hex << emulator.get_cpu().get_PC() << "\n";
#endif //DEBUG
//
//    std::cout << "Font Check\n";
//    for(int i = 0; i < 5; ++i)
//        printf("%04X: %02X\n", 0x050 + i, emulator.get_memory().raw()[0x050+i]);
//
//    for(int i = 0; i < 16; ++i)
//        printf("%04X: %02X\n", 0x200 + i, emulator.get_memory().raw()[0x200+i]);
//    emulator.tick();
//    emulator.tick();
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            gui.handle_event(event);
        }

        float dt = clock.restart().asSeconds();
        fps = 1.f / dt;
        fps_text.setString(std::to_string(static_cast<int>(fps)));
        emulator.tick();

        window.clear(sf::Color::Black);
        gui.update();
        gui.render();
        window.draw(fps_text);
        window.display();
    }

    return 0;
}
