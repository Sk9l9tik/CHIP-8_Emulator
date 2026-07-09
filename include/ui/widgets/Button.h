#ifndef CHIP_8_EMULATOR_BUTTON_H
#define CHIP_8_EMULATOR_BUTTON_H

#include <functional>
#include <utility>
#include "ui/Widget.h"

class Button : public Widget{
public:
    enum class State{
        Undefined,
        Normal,
        Locked,
        Hovered,
        Pressed,
    };

    /* Default ugly button constructor
     * To make it look better, you need to set 4 textures for all main states
     * */
    Button(const std::string& btn_text, sf::Vector2f size, sf::Vector2f pos, sf::Font &btn_font) : font(btn_font){
        if(size.x == 0 || size.y == 0){
            size = {1,1};
        }

        // TEXT
        text_size = static_cast<uint32_t>(size.x/4.f);
        text = sf::Text(font, btn_text, text_size);
        text.setFillColor(sf::Color::Black);

        // CLICK AREA
        bounds = sf::FloatRect(pos, size);

        // TEXTURES
        textures[State::Normal] = sf::RenderTexture{static_cast<sf::Vector2u>(size)};
        textures[State::Hovered] = sf::RenderTexture{static_cast<sf::Vector2u>(size)};
        textures[State::Pressed] = sf::RenderTexture{static_cast<sf::Vector2u>(size)};

        textures[State::Normal].clear(sf::Color::Transparent);
        textures[State::Hovered].clear(sf::Color::Transparent);
        textures[State::Pressed].clear(sf::Color::Transparent);

        sf::RectangleShape rect(size);
        rect.setFillColor(sf::Color::White);
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(0);

        textures[State::Normal].draw(rect);
        textures[State::Normal].display();

        rect.setFillColor({255,255,255,200} );

        textures[State::Hovered].draw(rect);
        textures[State::Hovered].display();

        rect.setFillColor({64,64,64,255});

        textures[State::Pressed].draw(rect);
        textures[State::Pressed].display();

        // SPRITE
        background.setTexture(textures[State::Normal].getTexture(), true);
        background.setPosition(pos);
        background.setScale({1,1});

        // TEXT CENTERING
        sf::FloatRect text_bounds = text.getLocalBounds();
        text.setOrigin({text_bounds.size.x / 2.f, text_bounds.size.y / 2.f});

        sf::FloatRect sprite_bounds = background.getGlobalBounds();
        text.setPosition({
            sprite_bounds.position.x + sprite_bounds.size.x / 2.f,
            sprite_bounds.position.y + sprite_bounds.size.y / 2.f - 2
        });

        //
        state = State::Normal;
    }


    /* Copy-constructor
     * */
    Button(const Button& other)
        : Widget(other),
        state(other.state),
        bounds(other.bounds),
        font(other.font),
        text_size(other.text_size),
        on_click(other.on_click),
        on_release(other.on_release)
    {
        text = other.text;
        text.setFont(font);
        text.setFillColor(other.text.getFillColor());

        background = other.background;
        background.setTexture(textures[state].getTexture(), true);
    }

    // Zachem ya na angliyskom pishy voobshe
    ~Button() override = default;

    /* Handles mouse events
     * */
    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i &mouse_pos) override {
        // Lock check
        if(is_locked()) return;

        // Hover check
        if(const auto& ev = event->getIf<sf::Event::MouseMoved>()){
            const bool hover = bounds.contains(static_cast<sf::Vector2f>(ev->position));
            if( hover && state != State::Hovered && state != State::Pressed){
                state = State::Hovered;
            } else if(!hover && state == State::Hovered) {
                state = State::Normal;
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
        try{
            background.setTexture(textures.at(state).getTexture(), true);
        } catch (std::exception& e){
            try {
                background.setTexture(textures.at(State::Normal).getTexture(), true);
            } catch (std::exception& e){
                throw std::runtime_error("Button have no textures.");
            }
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

        center_text();
    }

    void set_size(sf::Vector2f _size) override {
        size = _size;
        bounds.size = _size;

        text.setCharacterSize(static_cast<uint32_t>(_size.x/4.f));
        center_text();
    }

    bool load_texture(const sf::Texture& texture, State _state){
        sf::Vector2u texture_size = texture.getSize();
        sf::Vector2f button_size = bounds.size;

        sf::Sprite sprite(texture);
        sprite.setScale({
            button_size.x / static_cast<float>(texture_size.x),
            button_size.y / static_cast<float>(texture_size.y)
        });

        if (!textures[_state].resize(static_cast<sf::Vector2u>(button_size))) {
            return false;
        }

        textures[_state].clear(sf::Color::Transparent);
        textures[_state].draw(sprite);
        textures[_state].display();

        return true;
    }

    bool load_texture(const std::string& path, State _state){
        sf::Texture texture;
        if (!texture.loadFromFile(path)) {
            return false;
        }
        sf::Vector2u texture_size = texture.getSize();
        sf::Vector2f button_size = bounds.size;

        sf::Sprite sprite(texture);
        sprite.setScale({
            button_size.x / static_cast<float>(texture_size.x),
            button_size.y / static_cast<float>(texture_size.y)
        });

        if (!textures[_state].resize(static_cast<sf::Vector2u>(button_size))) {
            return false;
        }

        textures[_state].clear(sf::Color::Transparent);
        textures[_state].draw(sprite);
        textures[_state].display();

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

    void set_string(const std::string &_text){
        text.setString(_text);
    }


private:
    void center_text(){
        sf::FloatRect text_bounds = text.getLocalBounds();
        text.setOrigin({text_bounds.size.x / 2.f, text_bounds.size.y / 2.f});

        text.setPosition({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f - 4.f
        });
    }

    State state = State::Undefined;

    sf::FloatRect bounds;

    std::unordered_map<State, sf::RenderTexture> textures;

    sf::Texture dummy = {}; // constructor for 'Button' must explicitly initialize... OK
    sf::Sprite background = sf::Sprite(dummy);
    sf::Font &font;
    sf::Text text = {font,"",0};
    uint32_t text_size;

    std::function<void()> on_click;
    std::function<void()> on_release;
};

#endif //CHIP_8_EMULATOR_BUTTON_H
