#include "App.h"


App::App(const std::string& rom_path_):
    emulator(),
    debugger(emulator),
    rom_path(rom_path_),
    window(),
    font(),
    gui(window, emulator, debugger),
    sound_buffer(create_beep_buffer()),
    sound(sound_buffer),
    display(emulator.get_frame_buffer()),
    keyboard(4,4),
    keys(),
    d_labels(2,1),
    state_label("RUNNING", {100, 30}, {0,0}),
    PC_label("PC=0x0200", {100,30}, {0,0}),
    registers_table(4,4),
    reg_label("Registers", {80,20}, {0,0}),
    reg_labels(),
    spec_reg(2,2),
    I_label("I=0x000", {40,40}, {0,0}),
    SP_label("SP=0x00", {40,40}, {0,0}),
    DT_label("DT=0x00", {40,40}, {0,0}),
    ST_label("ST=0x00", {40,40}, {0,0}),
    open_file_btn("Open ROM", {130, 50}, {0,0}),
    disassembly_label("Disassembly", {100,20}, {0,0}),
    disassembly({1,1}, {1,1})
    {
    load_rom(rom_path);
    load_font();

    gui.setup(); // Checks dependencies (is font loaded, or other resources)

    setup_sound();
    //GUI
    setup_display();
    setup_keyboard();
    setup_debug_panel();
    setup_disassembly_panel();
    setup_open_rom_button();
    setup_debug_buttons();
    // SETUP ALL GUI ELEMENTS BEFORE WINDOW CREATION
    create_window();

    //debugger.pause();
}

void App::load_rom(const std::string &path) {
    try{
        emulator.load_ROM(path);
        rom_path = path;
        rom_size = std::filesystem::file_size(path);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to load ROM: ") + e.what());
    }
}

void App::load_font(const std::string& filename) {
    if (!font.openFromFile("../assets/fonts/" + filename))
        if(!font.openFromFile("../../assets/fonts/" + filename))
            throw std::runtime_error("Failed to load font: " + filename);

    ResourceManager::set_font(font);
}

sf::SoundBuffer App::create_beep_buffer() {
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
        throw std::runtime_error("Sound system error: failed to load beep samples");
    }

    return buffer;
}

void App::setup_sound() {
    sound.setLooping(true);
    sound.setVolume(10);
}

void App::setup_display() {
    display.set_size({640, 320});
    display.set_colors(0xA796E8FF, 0x181825FF);
    gui.add(&display);
}

void App::setup_keyboard() {
    auto d_pos = display.get_size();

    keyboard.set_position({0, d_pos.y});
    keyboard.set_size({640, 640});
    keyboard.style.gap.vertical = 64./3.;
    keyboard.style.gap.horizontal = 64./3.;
    keyboard.style.padding = {32};

    ResourceManager::load_texture("btn_keypad_hovered", "../assets/sprites/KBButton_Hovered.png");
    ResourceManager::load_texture("btn_keypad_pressed", "../assets/sprites/KBButton_Pressed.png");
    ResourceManager::load_texture("btn_keypad_normal", "../assets/sprites/KBButton_Default.png");
    ResourceManager::load_texture("btn_keypad_locked", "../assets/sprites/KBButton_Locked.png");

    for(int i = 0; i < 16; i++){
        keys[i] = Button{std::string(1, HEX_CHARS[i])};

        keys[i].set_size({128, 128});
        keys[i].set_char_size(32);
        keys[i].set_texture("btn_keypad_normal", Button::State::Normal);
        keys[i].set_texture("btn_keypad_hovered", Button::State::Hovered);
        keys[i].set_texture("btn_keypad_locked", Button::State::Locked);
        keys[i].set_texture("btn_keypad_pressed", Button::State::Pressed);

        keys[i].set_on_click([this, i](){
            emulator.get_cpu().set_key_state(i, true);
        });
        keys[i].set_on_release([this, i](){
            emulator.get_cpu().set_key_state(i, false);
        });
    }

    for(auto& k : keys){
        keyboard.add_widget(&k);
    }
    //keyboard.set_bg_color(0x2E2940FF);
    gui.add(&keyboard);
}

void App::setup_debug_panel() {
    auto d_pos = display.get_size();

    state_label.set_on_update([this](){
        state_label.set_string(debugger.is_paused() ? "PAUSED" : "RUNNING");
    });

    state_label.set_bg_color(0x2A2A3AFF);
    state_label.style.margin = {3};
    state_label.style.padding = {0,0,0,5};
    state_label.stick_to_left();
    PC_label.set_on_update([this](){
        PC_label.set_string("PC=0x"+utils::int_as_hex_str(debugger.get_cpu_state().PC, 4));
    });
    PC_label.set_bg_color(0x2A2A3AFF);
    PC_label.style.margin = {3, 3, 3, 0};
    PC_label.style.padding = {0,5,0,186};
    PC_label.set_text_color(0xffffffdd);
    PC_label.stick_to_left();
    state_label.set_text_color(0xED8653FF);
    state_label.set_char_size(21);
    PC_label.set_char_size(20);

    d_labels.set_position({d_pos.x + 10, 10});

    d_labels.add_widget(&state_label);
    d_labels.add_widget(&PC_label);

    d_labels.style.gap.horizontal = -3;

    d_labels.update();
    gui.add(&d_labels);

    reg_label.set_position({d_pos.x + 10, 60});
    reg_label.set_char_size(18);
    reg_label.set_text_color(0xffffffdd);

    gui.add(&reg_label);

    registers_table.set_position({d_pos.x + 10, reg_label.get_position().y + reg_label.get_size().y + 5});

    for(int i = 0; i < 16; i++){
        auto& l = reg_labels[i];
        l = {"VV=0x00", {}, {}};
        l.style.margin = {3};
//        l.stick_to_center(); // BY DEFAULT
        l.set_bg_color(0x2A2A3AFF);
        l.set_size(sf::Text(font, "VF=0x00", 20).getLocalBounds().size + sf::Vector2f{20.f, 20.f});
        l.set_char_size(20);
        l.set_on_update([&l, this, i](){
            l.set_string("V"+std::string(1, HEX_CHARS[i])+"=0x"+utils::int_as_hex_str(debugger.get_cpu_state().V[i]));
        });

        registers_table.add_widget(&l);
    }
    registers_table.update();
    registers_table.style.gap ={-2}; // На удивление это работает так, как я и думал
    gui.add(&registers_table);

    auto configure_reg_label = [this](Label& l){
        l.style.margin = {3.f};
        l.set_bg_color(0x2A2A3AFF);
        l.set_size(sf::Text(font, "I=0x000", 20).getLocalBounds().size + sf::Vector2f {20.f, 20.f});
        l.set_char_size(20);
    };
    configure_reg_label(I_label);
    configure_reg_label(SP_label);
    configure_reg_label(DT_label);
    configure_reg_label(ST_label);

    I_label.set_on_update([this](){
        I_label.set_string("I=0x" + utils::int_as_hex_str(debugger.get_cpu_state().I, 3));
    });
    SP_label.set_on_update([this](){
        SP_label.set_string("SP=0x" + utils::int_as_hex_str(debugger.get_cpu_state().SP));
    });
    DT_label.set_on_update([this](){
        DT_label.set_string("DT=0x" + utils::int_as_hex_str(debugger.get_cpu_state().DT));
    });
    ST_label.set_on_update([this](){
        ST_label.set_string("ST=0x" + utils::int_as_hex_str(debugger.get_cpu_state().ST));
    });

    spec_reg.set_position({d_pos.x + 10, registers_table.get_size().y+registers_table.get_position().y});

    spec_reg.add_widget(&I_label);
    spec_reg.add_widget(&SP_label);
    spec_reg.add_widget(&DT_label);
    spec_reg.add_widget(&ST_label);
    spec_reg.style.gap = {-2.f};
    spec_reg.update();
    gui.add(&spec_reg);

}

void App::setup_disassembly_panel(){
    auto d_pos = display.get_size();

    disassembly_label.set_string("Disassembly");
    disassembly_label.set_char_size(18);
    disassembly_label.set_position({d_pos.x + 10.f, spec_reg.get_size().y + spec_reg.get_position().y + 20.f});
    disassembly_label.stick_to_left();
    disassembly_label.set_text_color(0xffffffdd);
    gui.add(&disassembly_label);

    disassembly.set_size({registers_table.get_size().x - 6.f, 400});
    disassembly.set_position({d_pos.x + 10.f,
                              disassembly_label.get_size().y + disassembly_label.get_position().y + 5.f});
    disassembly.set_bg_color(0x313244FF);
    update_disassembly_panel();

    gui.add(&disassembly);
}

void App::update_disassembly_panel(){
    disassembly.clear();
    dsms.clear();

    for(uint32_t i = 0; i < rom_size / 2; i++){
        dsms.push_back(std::make_unique<Label>("0",
                                               sf::Vector2f{disassembly.get_size().x,36.f},
                                               sf::Vector2f{0.f,40.f * static_cast<float>(i)}));

        dsms[i]->set_char_size(22);
        dsms[i]->set_bg_color(0x2A2A3AFF);
        dsms[i]->set_text_color(sf::Color::White);
        dsms[i]->style.padding.left = 10.f;
        dsms[i]->stick_to_left();
        dsms[i]->set_on_update([&, i](){
            auto breakpoints = debugger.get_breakpoints().all();

            uint32_t mem_pos = 0x200 + 2*i;
            auto disasm = debugger.disassemble(mem_pos, 1);

            bool bp_set =  breakpoints.contains(disasm[0].address);
            std::string res = bp_set ? "* " : "";

            res += "0x"+utils::int_as_hex_str(disasm[0].address, 4) +
                    " " +utils::int_as_hex_str(disasm[0].opcode, 4) +
                    " " +disasm[0].mnemonic;

            dsms[i]->set_string(res);
            if(curr_addr == disasm[0].address ) { //&& debugger.is_paused()
                // Оно там так скачет, что будто и смысла нет без паузы
                dsms[i]->set_bg_color(0x17A62FFF);
            } else {
                dsms[i]->set_bg_color(bp_set ? 0xB57200FF  : 0x2A2A3AFF);
            }
        });

        dsms[i]->set_on_click([&, i](){
            uint32_t mem_pos = 0x200 + 2*i;
            debugger.toggle_breakpoint(mem_pos);
        });

        disassembly.add_widget(dsms[i].get());
    }
}

void App::track_current_instruction() {
    constexpr float line_height = 40.f; 

    uint16_t pc = debugger.get_cpu_state().PC;

    if (pc >= 0x202)
        pc -= 2;

    uint32_t index = (pc - 0x200) / 2;

    float target = index * line_height - disassembly.get_size().y * 0.5f + line_height * 0.5f;

    disassembly.set_scroll(target);

}

void App::setup_open_rom_button() {
    auto d_pos = display.get_size();
    //
    open_file_btn.set_position({d_pos.x + 10, disassembly.get_size().y + disassembly.get_position().y + 10});

    sf::RectangleShape rec(open_file_btn.get_size());
    rec.setFillColor(sf::Color::White);
    rec.setOutlineColor(sf::Color(0x999999ff));
    rec.setOutlineThickness(-2.f);
    sf::RenderTexture r_t(static_cast<sf::Vector2u>(rec.getSize()));
    r_t.draw(rec);
    r_t.display();
    ResourceManager::load_texture("def", r_t.getTexture());
    for(int i = 0; i < 5; i++){
        open_file_btn.set_texture("def", static_cast<Button::State>(i));
    }

    open_file_btn.set_char_size(18);
    open_file_btn.set_on_click([this](){
        open_rom_dialog();
    });
    gui.add(&open_file_btn);
}

void App::setup_debug_buttons(){
    auto k_pos = keyboard.get_position() + keyboard.get_size();

    const float DBG_BTN_HEIGHT = 30.f;

    debug_buttons[0] = {"F5 continue",
                        {100.f, DBG_BTN_HEIGHT},
                        {k_pos.x + 10.f, k_pos.y - DBG_BTN_HEIGHT - 10.f}};
    debug_buttons[1] = {"F9 pause",
                        {80.f, DBG_BTN_HEIGHT},
                        {0,0}};
    debug_buttons[2] = {"F10 step",
                        {80.f, DBG_BTN_HEIGHT},
                        {0,0}};
    debug_buttons[3] = {"toggle bp",
                        {100.f, DBG_BTN_HEIGHT},
                        {0,0}};

    for(auto& b : debug_buttons){
        for(int i = 0; i < 5; i++){
            b.set_texture("def", static_cast<Button::State>(i));
        }
    }

    for(int i = 1; i < 4; i++){
        float tmp = debug_buttons[i-1].get_size().x;// + debug_buttons[i-1].get_position();
        debug_buttons[i].set_position(debug_buttons[i-1].get_position() + sf::Vector2f{10.f + tmp,0.f});
    }

    if(debugger.is_paused()){
        debug_buttons[1].lock();
    } else {
        debug_buttons[0].lock();
        debug_buttons[2].lock();
    }

    debug_buttons[0].set_on_click([&](){
        debugger.resume();
//        debug_buttons[0].lock();
//        debug_buttons[1].unlock();
//        debug_buttons[2].lock();
    });
    debug_buttons[1].set_on_click([&](){
        debugger.pause();
//        debug_buttons[1].lock();
//        debug_buttons[0].unlock();
//        debug_buttons[2].unlock();
    });

    debug_buttons[0].set_on_update([&](){
        if(debugger.is_paused()){
            debug_buttons[1].lock();
            debug_buttons[0].unlock();
            debug_buttons[2].unlock();
        } else {
            debug_buttons[1].unlock();
            debug_buttons[0].lock();
            debug_buttons[2].lock();
        }
    });

//    debug_buttons[1].set_on_update([&](){
//        if(debugger.is_paused()){
//            debug_buttons[1].lock();
//            debug_buttons[2].unlock();
//        } else {
//            debug_buttons[1].unlock();
//            debug_buttons[2].lock();
//        }
//    });

    debug_buttons[2].set_on_click([&](){
        debugger.step();
    });
    debug_buttons[3].set_on_click([&](){
        debugger.toggle_breakpoint(curr_addr); // не ну аче)
    });


    for(auto& b : debug_buttons){
        b.set_char_size(16);
        gui.add(&b);
    }
}

void App::open_rom_dialog() {
#ifdef _WIN32
    auto path = utils::show_file_dialog(window.getNativeHandle());
#else
    auto path = utils::show_file_dialog();
#endif
    std::cout << path << '\n';

    if(path.ends_with(".ch8") && std::filesystem::exists(path)) {
        emulator.get_cpu().reset();
        emulator.load_ROM(path);
        rom_path = path;
        rom_size = std::filesystem::file_size(path);
        update_disassembly_panel();
        set_window_title(path);
    }
}

void App::set_window_title(const std::string &path) {
    window.setTitle(make_title(path));
}

std::string App::make_title(const std::string &path) {
    auto pos = path.find_last_of('/');
    if(pos == std::string::npos) pos = path.find_last_of('\\');
    return "CHIP-8 Emulator - " + path.substr(pos != std::string::npos ? pos + 1 : 0);
}

void App::create_window() {
    window.create(
            sf::VideoMode(static_cast<sf::Vector2u>(gui.get_size())),
            make_title(rom_path),
            // sf::Style::Close,
            sf::State::Windowed);
    window.setFramerateLimit(60);
    window.requestFocus();
}

void App::run(){
    cpu_last_time = cpu_clock.getElapsedTime();
    cpu_accumulator = sf::Time::Zero;
    timer_accumulator = sf::Time::Zero;

    const sf::Time CPU_TICK_TIME = sf::seconds(1.0f / CPU_HZ);
    const sf::Time CLOCK_TIME = sf::seconds(1.0f / TIMER_HZ);

    while(window.isOpen()){
        curr_addr = emulator.get_cpu().get_PC();
        // Текущее время работы CPU

        sf::Time curr = cpu_clock.getElapsedTime();
        // Разница с прошлым временем работы, т.е.
        // время прошедшее с последнего кадра
        sf::Time delta = curr - cpu_last_time;
        // сохраняем для след. кадра
//        std::cout << "SAERMO: " << test.getElapsedTime().asMilliseconds() << '\n';
//        std::cout << &cpu_clock << '\n';
//        std::cout << &test << '\n';

        cpu_last_time = curr;
        // накопитель
        cpu_accumulator += delta;
        timer_accumulator += delta;

        handle_events();

        update_cpu(CPU_TICK_TIME);
        update_timers(CLOCK_TIME);


        render();
    }
}

void App::handle_events() {
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
        gui.handle_event(event);
    }
}

void App::update_cpu(sf::Time cpu_tick_time) {
    while(cpu_accumulator >= cpu_tick_time) {
      bool was_paused = debugger.is_paused();

      debugger.update();

      if (!was_paused && debugger.is_paused()) {
          track_current_instruction();
      }

      cpu_accumulator -= cpu_tick_time;
    }
}

void App::update_timers(sf::Time clock_time) {
    while(timer_accumulator >= clock_time) {
        emulator.tick_timers();
        timer_accumulator -= clock_time;
    }
}

void App::update_sound_playback() {
    if(!debugger.is_paused()){
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
}

void App::render() {
    window.clear(sf::Color(0x0F0F18FF));

    static uint16_t last_pc = 0x200;

    uint16_t pc = debugger.get_cpu_state().PC;
    if (debugger.is_paused() && pc != last_pc) {
        track_current_instruction();
        last_pc = pc;
    }

    gui.update();
    gui.render();
    update_sound_playback();
    window.display();
}
