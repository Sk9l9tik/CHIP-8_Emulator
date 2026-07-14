#include <SFML/Audio.hpp>
#include <cmath>

#include <filesystem>
#include <iostream>

#include "utils.hpp"

#include "core/CHIP_8.h"
#include "core/Memory.h"

#include "ui/GUI.h"
#include "ui/ResourceManager.h"

#include "ui/widgets/Button.h"
#include "ui/widgets/Display.h"
#include "ui/widgets/Label.h"
#include "ui/widgets/ScrollView.h"
#include "ui/widgets/Table.h"
#include "ui/widgets/Toggle.h"

#include "debug/Debugger.h"


int main(int argc, char* argv[]){
    std::string rom_path{};

    if(argc < 2){
//        std::cerr << "Usage: " << argv[0] << " <path_to_rom>\n";
//        std::cerr << "Example: " << argv[0] << " Tetris_.ch8\n";
        rom_path = "../assets/roms/Tetris_.ch8";
        // return -1;
    }
    else {
        rom_path = argv[1];
    }

    CHIP_8 emulator {};
    Debugger d(emulator);

    try{
        emulator.load_ROM(rom_path);
        //std::cout << "Loaded ROM: " << rom_path << '\n';
    } catch(const std::exception& e){
        std::cerr << "Failed to load ROM: "<< e.what() << '\n';
        return -1;
    }


    sf::RenderWindow window;

    GUI gui{&window, &emulator, &d};
    sf::Font font;
    if (!font.openFromFile("../assets/fonts/Roboto-Regular.ttf"))
        if(!font.openFromFile("../../assets/fonts/Roboto-Regular.ttf"))
           return -1;

    ResourceManager::set_font(font);

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
    keyboard.style.gap.vertical = 64./3.;
    keyboard.style.gap.horizontal = 64./3.;
    keyboard.style.padding = {32};
    std::vector<Button> keys;
    keys.reserve(16);

    ResourceManager::load_texture("btn_keypad_normal", "../assets/sprites/KBButton_Default.png");
    ResourceManager::load_texture("btn_keypad_hovered", "../assets/sprites/KBButton_Hovered.png");
    ResourceManager::load_texture("btn_keypad_pressed", "../assets/sprites/KBButton_Pressed.png");
    ResourceManager::load_texture("btn_keypad_locked", "../assets/sprites/KBButton_Locked.png");
    const char hex_chars[] = "0123456789ABCDEF";
    for(int i = 0; i < 16; i++){
        keys.emplace_back(std::string(1,hex_chars[i]));

        keys[i].set_size({128, 128});
        keys[i].set_char_size(32);
        keys[i].set_texture("btn_keypad_normal", Button::State::Normal);
        keys[i].set_texture("btn_keypad_hovered", Button::State::Hovered);
        keys[i].set_texture("btn_keypad_locked", Button::State::Locked);
        keys[i].set_texture("btn_keypad_pressed", Button::State::Pressed);

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
    //keyboard.set_bg_color(0x2E2940FF);
    gui.add(&keyboard);


    sf::Clock cpu_clock;
    const float CPU_HZ = 700.0f; // Частота проца
    const sf::Time CPU_TICK_TIME = sf::seconds(1.0f / CPU_HZ);
    sf::Time cpu_accumulator = sf::Time::Zero;
    sf::Time cpu_last_time = cpu_clock.getElapsedTime();

//    sf::Clock timer_clock;
    const sf::Time CLOCK_TIME = sf::seconds(1.0f / 60.f);
    sf::Time timer_accumulator = sf::Time::Zero;
//    sf::Time last_time = timer_clock.getElapsedTime();

    Table d_labels{2,1};
    Label state_label{"RUNNING", {100, 30}, {0,0}};
    state_label.set_on_update([&state_label, &d](){
        state_label.set_string(d.is_paused() ? "PAUSED" : "RUNNING");
    });
    //state_label.set_bg_color(0x2A2A3AFF);
    state_label.style.padding = {3,190,3,3};
    Label PC_label{"PC=0x0200", {100, 30}, {0,0}};
    PC_label.set_on_update([&PC_label, &d](){
        PC_label.set_string("PC=0x"+utils::int_as_hex_str(d.get_cpu_state().PC, 4));
    });
    //PC_label.set_bg_color(0x2A2A3AFF);
    PC_label.style.padding = {3,3,3,0};
    PC_label.set_text_color(0xffffffdd);
    state_label.set_text_color(0xED8653FF);
    state_label.set_char_size(21);
    PC_label.set_char_size(20);

    d_labels.set_position({d_pos.x + 10, 10});

    d_labels.add_widget(&state_label);
    d_labels.add_widget(&PC_label);

    d_labels.update();

    gui.add(&d_labels);

    Label reg_label("Registers", {80, 20}, {d_pos.x + 10, 60});
    reg_label.set_char_size(18);
    reg_label.set_text_color(0xffffffdd);
    gui.add(&reg_label);

    Table registers{4, 4};
    registers.set_position({d_pos.x + 10, reg_label.get_position().y + reg_label.get_size().y + 5});
    std::array<Label, 16> labels;

    for(int i = 0; i < 16; i++){
        auto& l = labels[i];
        l = {"VV=0x00", {}, {}};
        l.style.padding = {3};
        l.set_bg_color(0x2A2A3AFF);
        l.set_size(sf::Text(font, "VF=0x00", 20).getLocalBounds().size + sf::Vector2f{20.f, 20.f});
        l.set_char_size(20);
        l.set_on_update([&l, &d, i, hex_chars](){
            l.set_string("V"+std::string(1, hex_chars[i])+"=0x"+utils::int_as_hex_str(d.get_cpu_state().V[i]));
        });

        registers.add_widget(&l);
    }
    registers.update();
    registers.style.gap ={-2}; // На удивление это работает так, как я и думал
    gui.add(&registers);
    // TODO: сделать смену Origin у текста, чтобы можно было прибить его к левому краю
    Table spec_reg{2,2};
    spec_reg.set_position({d_pos.x + 10, registers.get_size().y+registers.get_position().y});
    Label I_reg_label("I=0x000", {40,40}, {0,0});
    Label SP_reg_label("SP=0x00", {40,40}, {0,0});
    Label DT_reg_label("DT=0x00", {40,40}, {0,0});
    Label ST_reg_label("ST=0x00", {40,40}, {0,0});
    I_reg_label.style.padding = {3.f};
    SP_reg_label.style.padding = {3.f};
    DT_reg_label.style.padding = {3.f};
    ST_reg_label.style.padding = {3.f};

    I_reg_label.set_bg_color(0x2A2A3AFF);
    SP_reg_label.set_bg_color(0x2A2A3AFF);
    DT_reg_label.set_bg_color(0x2A2A3AFF);
    ST_reg_label.set_bg_color(0x2A2A3AFF);
    I_reg_label.set_size(sf::Text(font, "I=0x000", 20).getLocalBounds().size + sf::Vector2f{20.f, 20.f});
    I_reg_label.set_char_size(20);
    I_reg_label.set_on_update([&I_reg_label, &d](){
        I_reg_label.set_string("I=0x" + utils::int_as_hex_str(d.get_cpu_state().I, 3));
    });
    SP_reg_label.set_size(sf::Text(font, "I=0x000", 20).getLocalBounds().size + sf::Vector2f{20.f, 20.f});
    SP_reg_label.set_char_size(20);
    SP_reg_label.set_on_update([&SP_reg_label, &d](){
        SP_reg_label.set_string("SP=0x" + utils::int_as_hex_str(d.get_cpu_state().SP));
    });
    DT_reg_label.set_size(sf::Text(font, "I=0x000", 20).getLocalBounds().size + sf::Vector2f{20.f, 20.f});
    DT_reg_label.set_char_size(20);
    DT_reg_label.set_on_update([&DT_reg_label, &d](){
        DT_reg_label.set_string("DT=0x" + utils::int_as_hex_str(d.get_cpu_state().DT));
    });
    ST_reg_label.set_size(sf::Text(font, "I=0x000", 20).getLocalBounds().size + sf::Vector2f{20.f, 20.f});
    ST_reg_label.set_char_size(20);
    ST_reg_label.set_on_update([&ST_reg_label, &d](){
        ST_reg_label.set_string("ST=0x" + utils::int_as_hex_str(d.get_cpu_state().ST));
    });

    spec_reg.add_widget(&I_reg_label);
    spec_reg.add_widget(&SP_reg_label);
    spec_reg.add_widget(&DT_reg_label);
    spec_reg.add_widget(&ST_reg_label);
    spec_reg.style.gap = {-2.f};
    spec_reg.update();
    gui.add(&spec_reg);

    Button open_file{"Open ROM", {130, 50}, {d_pos.x + 10, spec_reg.get_size().y+spec_reg.get_position().y + 10}};
    sf::RectangleShape rec(open_file.get_size());
    rec.setFillColor(sf::Color::White);
    rec.setOutlineColor(sf::Color(0x999999ff));
    rec.setOutlineThickness(-2.f);
    sf::RenderTexture r_t(static_cast<sf::Vector2u>(rec.getSize()));
    r_t.draw(rec);
    r_t.display();
    ResourceManager::load_texture("def", r_t.getTexture());
    for(int i = 0; i < 5; i++){
        open_file.set_texture("def", static_cast<Button::State>(i));
    }

    open_file.set_char_size(18);
    open_file.set_on_click([&window, &emulator, &rom_path](){

#ifdef _WIN32
        auto path = utils::show_file_dialog(window.getNativeHandle());
#else
        auto path = utils::show_file_dialog();
#endif
        std::cout << path << '\n';

        if(path.ends_with(".ch8") && std::filesystem::exists(path)) {
            emulator.get_cpu().reset();
            emulator.load_ROM(path);

            auto pos = path.find_last_of('/');
            if(pos == std::string::npos) pos = path.find_last_of('\\');
            window.setTitle("CHIP-8 Emulator - " + path.substr(pos != std::string::npos ? pos+1 : 0));

            rom_path = path;
        }
    });
    gui.add(&open_file);

    auto pos = rom_path.find_last_of('/');
    if(pos == std::string::npos) pos = rom_path.find_last_of('\\');

    // ADD ALL GUI ELEMENTS BEFORE WINDOW CREATION
    window.create(
            sf::VideoMode(static_cast<sf::Vector2u>(gui.get_size())),
                    "CHIP-8 Emulator - " + rom_path.substr(pos != std::string::npos ? pos+1 : 0),
                    sf::Style::Default,
                    sf::State::Windowed);
    window.setFramerateLimit(60);


    //printf("%f\n", registers.get_size().x);
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
        cpu_accumulator += delta;
        timer_accumulator += delta;


        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            gui.handle_event(event);
        }

        while(cpu_accumulator >= CPU_TICK_TIME) {
            d.update();
            cpu_accumulator -= CPU_TICK_TIME;
        }

        while(timer_accumulator >= CLOCK_TIME) {
            emulator.tick_timers();
            timer_accumulator -= CLOCK_TIME;
        }

        window.clear(sf::Color(0x0F0F18FF));
        gui.update();
        gui.render();

        if(!d.is_paused()){
            sound.setVolume(10);
            if (emulator.get_cpu().get_sound_timer() > 0) {
                if (sound.getStatus() != sf::Sound::Status::Playing) {
                    sound.play();
                }
            } else {
                if (sound.getStatus() == sf::Sound::Status::Playing) {
                    sound.stop();
                }
            }
        } else {
            sound.setVolume(0);
        }

        window.display();
    }

    return 0;
}
