#ifndef CHIP_8_EMULATOR_UTILS_HPP
#define CHIP_8_EMULATOR_UTILS_HPP

// Весь winapi в namespace utils был 0_o
#if defined(_WIN32)
#include <windows.h>
#else

#ifdef HAVE_NFD
#include <nfd.hpp>
#endif
#endif

#include <iostream>

#include <SFML/Graphics.hpp>

namespace utils {
inline void SAERMO_logger(const std::optional<sf::Event>& event) {
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

inline std::string int_as_hex_str(int value, int min_width = 2) {
    if (value == 0 && min_width == 0) return "0";

    const char hex_chars[] = "0123456789ABCDEF";
    std::string result;
    auto val = static_cast<uint32_t>(value);

    do {
        result += hex_chars[val % 16];
        val /= 16;
    } while (val > 0 || result.size() < static_cast<size_t>(min_width));

    std::reverse(result.begin(), result.end());
    return result;
}

// Replaces text in bounds
inline void center_text(sf::Text &text, sf::FloatRect bounds){
    sf::FloatRect text_bounds = text.getLocalBounds();

    text.setOrigin({
        text_bounds.size.x / 2.f,
        text_bounds.position.y + text_bounds.size.y / 2.f
    });

    text.setPosition({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    });
}

inline void left_text(sf::Text &text, sf::FloatRect bounds){
    sf::FloatRect text_bounds = text.getLocalBounds();

    text.setOrigin({
        text_bounds.position.x,
        text_bounds.position.y + text_bounds.size.y / 2.f
    });

    text.setPosition({
        bounds.position.x,
        bounds.position.y + bounds.size.y / 2.f
    });
}

inline void right_text(sf::Text &text, sf::FloatRect bounds){
    sf::FloatRect text_bounds = text.getLocalBounds();

    text.setOrigin({
        text_bounds.size.x + text_bounds.position.x,
        text_bounds.position.y + text_bounds.size.y / 2.f
    });

    text.setPosition({
        bounds.position.x + bounds.size.x,
        bounds.position.y + bounds.size.y / 2.f
    });
}
#ifdef _WIN32
inline std::wstring show_file_dialog_w(HWND hwnd) {
    OPENFILENAMEW ofn = {};
    wchar_t szFile[260] = {0};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Chip-8 ROM (*.ch8)\0*.ch8\0All files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {
        std::wstring path = szFile;
        return path;
    }

    return {};
}

inline std::string show_file_dialog(HWND hwnd) {
    OPENFILENAMEA ofn = {};
    char szFile[260] = {0};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Chip-8 ROM (*.ch8)\0*.ch8\0All files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        std::string path = szFile;
        return path;
    }
    return {};
}
#else  

inline std::string show_file_dialog() {
    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;

    nfdfilteritem_t filters[1] = {{"CHIP-8 ROMs", "ch8,c8,rom"}};
    nfdresult_t result = NFD::OpenDialog(outPath, filters, 1);

    if (result == NFD_OKAY) {
        return std::string(outPath.get());
    }
    return {};
}

#endif
}

#endif //CHIP_8_EMULATOR_UTILS_HPP
