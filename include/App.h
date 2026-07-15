#ifndef CHIP_8_EMULATOR_APP_H
#define CHIP_8_EMULATOR_APP_H

#include <string>
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

class App{
public:
    explicit App(const std::string& rom_path_);
    void run();

    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

private:
    void load_rom(const std::string& path);
    void load_font();
    void setup_sound();
    void setup_display();
    void setup_keyboard();
    void setup_debug_panel();
    void setup_open_rom_button();
    void create_window();

    void handle_events();
    void update_cpu(sf::Time cpu_tick_time);
    void update_timers(sf::Time clock_time);
    void update_sound_playback();
    void render();

    void open_rom_dialog();
    void set_window_title(const std::string& path);
    static std::string make_title(const std::string& path);

    static sf::SoundBuffer create_beep_buffer();

    static constexpr uint32_t MEM_START = 0x0200;
    static constexpr uint32_t MEM_END = 0x1000;
    static constexpr uint32_t usable_memory_size = (MEM_END - MEM_START) / 2; // По сути кол-во строк в Disassembly
    static constexpr char HEX_CHARS[] = "0123456789ABCDEF";
    static constexpr float CPU_HZ = 700.0f;
    static constexpr float TIMER_HZ = 60.0f;

    CHIP_8 emulator;
    Debugger debugger;
    std::string rom_path;

    sf::RenderWindow window;
    sf::Font font;
    GUI gui;

    sf::SoundBuffer sound_buffer;
    sf::Sound sound;

    Display display;
    Table keyboard;
    std::array<Button, 16> keys;

    Table d_labels;
    Label state_label;
    Label PC_label;
    Table registers_table;
    Label reg_label;
    std::array<Label, 16> reg_labels;
    Table spec_reg;
    Label I_label, SP_label, DT_label, ST_label;
    Button open_file_btn;
    Label disassembly_label;
    ScrollView disassembly;
    std::array<std::unique_ptr<Table>, usable_memory_size>  rows;
    std::array<std::unique_ptr<Toggle>, usable_memory_size> bps;
    std::array<std::unique_ptr<Label>, usable_memory_size>  dsms;


    sf::Clock cpu_clock;
    sf::Time cpu_accumulator{sf::Time::Zero};
    sf::Time cpu_last_time{sf::Time::Zero};
    sf::Time timer_accumulator{sf::Time::Zero};
};

#endif //CHIP_8_EMULATOR_APP_H
