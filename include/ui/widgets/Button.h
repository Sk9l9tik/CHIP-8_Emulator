#ifndef CHIP_8_EMULATOR_BUTTON_H
#define CHIP_8_EMULATOR_BUTTON_H

#include <functional>
#include <utility>
#include "ui/Widget.h"
#include "utils.hpp"

using utils::center_text;

class Button : public Widget{
public:
    enum class State{
        Undefined,
        Normal,
        Locked,
        Hovered,
        Pressed,
    };

    /* Transparent button constructor
     * To make it look better, you need to set 4 textures for all main states
     * And also set size, position and text settings n everything
     * */
    explicit Button(const std::string& btn_text)
            : text(ResourceManager::get_font(), btn_text, 12)
    {
        size = {64.f,64.f};
        pos  = {0.f,0.f};

        // DEFAULT TEXT SETTINGS
        text.setFillColor(sf::Color::Black);

        // CLICK AREA
        bounds = sf::FloatRect(pos, size);

        center_text(text, bounds);

        state = State::Normal;
    }

    Button(const std::string& btn_text, sf::Vector2f _size, sf::Vector2f _pos)
            : text(ResourceManager::get_font(), btn_text, 12)
    {
        size = _size;
        pos  = _pos;
        background.setPosition(_pos);
        bounds.position = _pos;

        // DEFAULT TEXT SETTINGS
        text.setFillColor(sf::Color::Black);

        // CLICK AREA
        bounds = sf::FloatRect(pos, size);

        center_text(text, bounds);

        state = State::Normal;
    }

    // Copy constructor
    Button(const Button& other)
            : Widget(other), bounds(other.bounds),
              textures(other.textures),
              text(other.text),
              on_click(other.on_click),
              on_release(other.on_release),
              on_mouse_entered(other.on_mouse_entered),
              on_mouse_exited(other.on_mouse_exited),
              while_pressed(other.while_pressed)
              {}

    // Default constructor
    Button() : Button("") {}

    ~Button() override = default;

    /* Handles mouse events */
    void handle_event(const std::optional<sf::Event> &event) override {
        // Lock check
        if(is_locked()) return;

        // Hover check
        if(const auto& ev = event->getIf<sf::Event::MouseMoved>()){
            const bool hover = bounds.contains(static_cast<sf::Vector2f>(ev->position));
            if( hover && state != State::Hovered && state != State::Pressed){
                state = State::Hovered;
                on_mouse_entered();
            } else if(!hover && state == State::Hovered) {
                state = State::Normal;
                on_mouse_exited();
            }
        }

        //Is pressed
        if(const auto& ev = event->getIf<sf::Event::MouseButtonPressed>()){
            if(ev->button == sf::Mouse::Button::Left && bounds.contains(static_cast<sf::Vector2f>(ev->position))){
                state = State::Pressed;
                on_click();
            }
        }

        //Is released
        if(const auto& ev = event->getIf<sf::Event::MouseButtonReleased>()){
            if(state == State::Pressed && ev->button == sf::Mouse::Button::Left){
                state = bounds.contains(static_cast<sf::Vector2f>(ev->position)) ? State::Hovered : State::Normal;
                on_release();
            }
        }
    }

    void update() override {
        if(state == State::Pressed) while_pressed();
        if(state == State::Hovered) while_hovered();

        try {
            auto t = textures.at(state);
            background.setTexture(*t, true);
            background.setScale({size.x / static_cast<float>((*t).getSize().x),
                                        size.y / static_cast<float>((*t).getSize().y)});
        } catch (std::exception& e){
            // Does he blow?

        }
    }

    void render(sf::RenderTarget &target) override {
        target.draw(background);
        target.draw(text);
    }

    void set_position(sf::Vector2f _pos) override {

        pos = _pos;
        bounds.position = _pos;
        background.setPosition(_pos);

        center_text(text, bounds);
    }

    void set_size(sf::Vector2f _size) override {
        if(_size == size) return;
        if(_size.x != 0 || _size.y != 0) {
            background.scale({_size.x / size.x, _size.y / size.y});
        } else {
            background.setScale({0.f, 0.f});
        }

        size = _size;
        bounds.size = _size;

        center_text(text, bounds);
    }

    bool set_texture(const std::string &texture_name, State for_state){
        auto texture = ResourceManager::get_texture(texture_name);
        if(!texture) return false;

        textures[for_state] = texture;

        return true;
    }

    void lock()   { state = State::Locked; }
    void unlock() { state = State::Normal; }

    bool is_locked() const { return state == State::Locked; }

    //xd
    void set_on_click(std::function<void()> func) {
        on_click = std::move(func);
    }

    void set_on_release(std::function<void()> func) {
        on_release = std::move(func);
    }

    void set_on_mouse_entered(std::function<void()> func){
        on_mouse_entered = std::move(func);
    }

    void set_on_mouse_exited(std::function<void()> func){
        on_mouse_exited = std::move(func);
    }

    void set_while_pressed(std::function<void()> func) {
        while_pressed = std::move(func);
    }

    void set_while_hovered(std::function<void()> func) {
        while_hovered = std::move(func);
    }


    void set_char_size(uint32_t new_size) {
        text.setCharacterSize(new_size);
        center_text(text, bounds);
    }

    void set_string(const std::string& new_str){
        text.setString(new_str);
        center_text(text, bounds);
    }

    void set_text_color(uint32_t color){
        text.setFillColor(sf::Color(color));
    }

    // анфортунатули, без этого никак
    void set_state(State s){
        State old = state;
        state = s;
        switch(old){
            case State::Normal:
                switch(s){
                    case State::Pressed:
                        on_click();
                        break;
                    case State::Hovered:
                        on_mouse_entered();
                        break;
                    default: break;
                }
                break;
            case State::Pressed:
                switch(s){
                    case State::Normal:
                    case State::Hovered:
                        on_release();
                        break;
                    default: break;
                }
                break;
            case State::Hovered:
                switch(s){
                    case State::Normal:
                        on_mouse_exited();
                        break;
                    case State::Pressed:
                        on_click();
                        break;
                    default: break;
                }
                break;
            default:
                break;
        }
    }
protected:
    // Button state
    State state = State::Undefined;

    // Click area
    sf::FloatRect bounds;

    // Textures
    std::unordered_map<State, const sf::Texture*> textures;

    sf::Texture dummy = {}; // constructor for 'Button' must explicitly initialize... OK (56 bytes ksta)
    sf::Sprite background = sf::Sprite(dummy);

    // Text
    sf::Text text;

    // Event handlers
    std::function<void()> on_click          = [](){};
    std::function<void()> on_release        = [](){};
    std::function<void()> on_mouse_entered  = [](){};
    std::function<void()> on_mouse_exited   = [](){};
    /* called every frame while button is pressed
     * */
    std::function<void()> while_pressed     = [](){};
    std::function<void()> while_hovered     = [](){};
};

#endif //CHIP_8_EMULATOR_BUTTON_H
