#include <SFML/Audio.hpp>
#include <cmath>

#include <iostream>
#include <filesystem>

#include "utils.hpp"

#include "core/CHIP_8.h"
#include "core/Memory.h"

#include "ui/GUI.h"

#include "ui/widgets/Button.h"
#include "ui/widgets/Display.h"
#include "ui/widgets/Table.h"
#include "ui/widgets/Toggle.h"

#define CHIP8_DEBUG1

#ifdef CHIP8_DEBUG
#include "debug/Debugger.h"
#endif

int main(int argc, char* argv[]){
    std::string rom_path{};

    if(argc < 2){
//        std::cerr << "Usage: " << argv[0] << " <path_to_rom>\n";
//        std::cerr << "Example: " << argv[0] << " Tetris_.ch8\n";
        rom_path = "Tetris_.ch8";
        // return -1;
    }
    else {
        rom_path = argv[1];
    }

    CHIP_8 emulator {};

    try{
        emulator.load_ROM(rom_path);
        //std::cout << "Loaded ROM: " << rom_path << '\n';
    } catch(const std::exception& e){
        std::cerr << "Failed to load ROM: "<< e.what() << '\n';
        return -1;
    }


    sf::RenderWindow window(
            sf::VideoMode({640, 960}),
            "CHIP-8 Emulator - " + rom_path,
            sf::Style::Default,
            sf::State::Windowed);

    window.setFramerateLimit(60);


    GUI gui{&window, &emulator};
    sf::Font font;
    if(!font.openFromFile("../assets/fonts/Galmuri7.ttf")) {
        return -1;
    }
    font.setSmooth(false);

    // SOUND
    const unsigned SAMPLE_RATE = 44100;
    const unsigned DURATION_SECONDS = 1;
    const std::size_t SAMPLE_COUNT = SAMPLE_RATE * DURATION_SECONDS;
    const float FREQUENCY = 440.f;
    const std::int16_t AMPLITUDE = 30000;

    std::vector<std::int16_t> samples(SAMPLE_COUNT);
    for (std::size_t i = 0; i < SAMPLE_COUNT; ++i) {
        double time = static_cast<double>(i) / SAMPLE_RATE;
        double value = AMPLITUDE * std::sin(2.0 * M_PI * FREQUENCY * time);
        samples[i] = static_cast<std::int16_t>(value);
    }

    sf::SoundBuffer buffer;
    std::vector<sf::SoundChannel> channelMap = { sf::SoundChannel::Mono };

    if (!buffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, channelMap)) {
        std::cerr << "Sound system error: Otshlefuy" << std::endl;
        return -1;
    }

    sf::Sound sound(buffer);
    sound.setLooping(true);
    sound.setVolume(10);
    // SOUND END

    Display display{emulator.get_frame_buffer()};
    display.set_size({640, 320});
    display.set_colors(0xA796E8FF, 0x181825FF);

    gui.add(&display);

    auto d_pos = display.get_size();

    Table keyboard{4,4};
    keyboard.set_position({0, d_pos.y});
    keyboard.set_size({640, 640});
    keyboard.vertical_gap = 21;
    keyboard.horizontal_gap = 21;
    keyboard.padding = {32};
    std::vector<Button> keys;
    keys.reserve(16);

    for(int i = 0; i < 16; i++){
        const char hex_chars[] = "0123456789ABCDEF";
        keys.emplace_back(Button{
            std::string(1,hex_chars[i]),
            {0,0},
            {0,0},
            font
        });

        keys[i].set_size({128, 128});

        keys[i].load_texture("../assets/sprites/KBButton_Default.png", Button::State::Normal);
        keys[i].load_texture("../assets/sprites/KBButton_Hovered.png", Button::State::Hovered);
        keys[i].load_texture("../assets/sprites/KBButton_Locked.png", Button::State::Locked);
        keys[i].load_texture("../assets/sprites/KBButton_Pressed.png", Button::State::Pressed);

        keys[i].set_on_click([&emulator, i](){
            emulator.get_cpu().set_key_state(i, true);
        });
        keys[i].set_on_release([&emulator, i](){
            emulator.get_cpu().set_key_state(i, false);
        });
    }

    for(auto& k : keys){
        keyboard.add_widget(&k);
    }

    gui.add(&keyboard);

#ifdef _WIN32
    Button open_file{"Open ROM", {150, 80}, {d_pos.x + 10, 0}, font};
    open_file.set_text_size(18);
    open_file.set_on_click([&window, &emulator](){
        auto path = utils::show_file_dialog(window.getNativeHandle());

        if(path.ends_with(".ch8") && std::filesystem::exists(path)) {
            emulator.get_cpu().reset();
            emulator.load_ROM(path);
        }
    });
    gui.add(&open_file);
#endif

    Toggle test{"", {50.f, 50.f}, {d_pos.x +10, 90}, font};
    sf::CircleShape btn_c(50.f/4.f, 30);

    btn_c.setFillColor(sf::Color::Red);
    btn_c.setPosition({test.get_size().x / 4.f, test.get_size().y / 4.f});

    sf::RenderTexture t(static_cast<sf::Vector2u>(test.get_size()));
    t.clear(sf::Color::Transparent);
    t.draw(btn_c);
    t.display();

    test.load_texture(t.getTexture(), Button::State::Pressed);

    btn_c.setFillColor({255,0,0,180});
    t.clear(sf::Color::Transparent);
    t.draw(btn_c);
    t.display();

    test.load_texture(t.getTexture(), Button::State::Hovered);

    sf::Text text(font, "123", 16);
    text.setFillColor(sf::Color::White);
    utils::center_text(text, {{}, {50.f, 50.f}});
    t.clear(sf::Color::Transparent);
    t.draw(text);
    t.display();

    test.load_texture(t.getTexture(), Button::State::Normal);

    test.set_on_click([](){
        printf("ON\n");
    });
    test.set_on_release([](){
        printf("OFF\n");
    });
    gui.add(&test);
#ifdef CHIP8_DEBUG
    Debugger d(emulator);

    auto disassembled = d.disassemble(Memory::ROM_START, 20);
    std::cout << "addr\topcode\tmnemonic\n";
    // for(auto&& [addr, opc, mnem] : disassembled) {
    //   std::cout << "0x" << std::hex << std::uppercase << addr << "\t" << "0x" << opc << "\t" << mnem << '\n';
    // }  

    // auto a = d.memory_dump(Memory::ROM_START, Memory::MEM_SIZE);
    // std::cout << a << '\n';  
    // d.pause(); // cause no-bp is not set
    d.set_breakpoint(0x20a);
    // std::cout << "Paused at PC = 0x" << std::hex << emulator.get_cpu().get_PC() << "\n";
#endif

    sf::Clock cpu_clock;
    const float CPU_HZ = 700.0f; // Частота проца
    const sf::Time CPU_TICK_TIME = sf::seconds(1.0f / CPU_HZ);
    sf::Time cpu_accumulator = sf::Time::Zero;
    sf::Time cpu_last_time = cpu_clock.getElapsedTime();

//    sf::Clock timer_clock;
    const sf::Time CLOCK_TIME = sf::seconds(1.0f / 60.f);
    sf::Time timer_accumulator = sf::Time::Zero;
//    sf::Time last_time = timer_clock.getElapsedTime();



    // \/\/\/
    window.requestFocus();

    while (window.isOpen())
    {
        // Текущее время работы CPU
        sf::Time curr = cpu_clock.getElapsedTime();
        // Разница с прошлым временем работы, т.е.
        // время прошедшее с последнего кадра
        sf::Time delta = curr - cpu_last_time;
        // сохраняем для след. кадра
        cpu_last_time = curr;
        // накопитель
        cpu_accumulator+=delta;
        timer_accumulator+=delta;
        //

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

                // TODO: move this 3 blocks to window parts
                else if(kp->scancode == sf::Keyboard::Scancode::F1){ // print cpu state
                    auto state = d.get_cpu_state();

                    std::cout << "PC: 0x" << std::hex << state.PC << '\n';
                    std::cout << "I : 0x" << state.I  << '\n';
                    std::cout << "SP: "   << std::dec << static_cast<int>(state.SP) << '\n';
                    std::cout << "DT: "   << static_cast<int>(state.DT) << '\n';
                    std::cout << "ST: "   << static_cast<int>(state.ST) << "\n\n";

                    std::cout << std::uppercase << std::hex << std::setfill('0');

                    auto registers = state.V;

                    for (std::size_t i = 0, end = registers.size(); i < end; ++i) {
                        std::cout << "V" << std::hex << i << ": 0x" << std::setw(2) << static_cast<int>(registers[i]) << '\n';
                    }
                    std::cout << std::dec;                          
                }
                else if(kp->scancode == sf::Keyboard::Scancode::F2){ // print memory
                    auto memory = d.memory_dump(Memory::ROM_START, Memory::MEM_SIZE);
                    std::cout << memory << '\n';  
                }
                else if(kp->scancode == sf::Keyboard::Scancode::F3){ // print stack
                    auto state = d.get_cpu_state();
                    std::cout << "Stack (SP = " << static_cast<int>(state.SP) << ")\n";

                        std::cout << std::uppercase << std::hex << std::setfill('0');

                        for (std::size_t i = 0; i < state.stack.size(); ++i) {
                            std::cout << '[' << std::setw(2) << i << "] " << "0x" << std::setw(4) << state.stack[i];

                            if (i == state.SP)
                                std::cout << "  <-- SP";

                            std::cout << '\n';
                        }

                        std::cout << std::dec;
                }
        }
#endif
#ifdef DEBUG
        utils::SAERMO_logger(event);
#endif

            gui.handle_event(event, sf::Mouse::getPosition(window));
        }

        while(cpu_accumulator >= CPU_TICK_TIME) {
#ifdef CHIP8_DEBUG
            d.update();
            if(d.is_paused() == false || true){}
#else
            emulator.tick();
#endif
            cpu_accumulator -= CPU_TICK_TIME;
        }

        while(timer_accumulator >= CLOCK_TIME){
            emulator.tick_timers();
            timer_accumulator -= CLOCK_TIME;
        }

        window.clear(sf::Color::Black);
        gui.update();
        gui.render();

        if(emulator.get_cpu().get_sound_timer() > 0){
            if (sound.getStatus() != sf::Sound::Status::Playing) {
                sound.play();
            }
        } else {
            if (sound.getStatus() == sf::Sound::Status::Playing) {
                sound.stop();
            }
        }

        window.display();
    }

    return 0;
}
