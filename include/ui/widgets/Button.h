#ifndef CHIP_8_EMULATOR_BUTTON_H
#define CHIP_8_EMULATOR_BUTTON_H

#include <functional>
#include "ui/Widget.h"

/* Чистить и рефакторить весь бред который я тут написал я буду только тогда
 * когда все это каким то образом начнет работать
 *
 * этот прикол готов дай бог на половину есче
 * кто тут TODO
 * писать будет - в рот нассу
 * */

class Button : public Widget{
public:
    enum class State{
        Undefined,
        Normal,
        Locked,
        Hovered,
        Pressed,
    };

    //Test button
    Button(const std::string& btn_text, sf::Vector2f size, sf::Vector2f pos, sf::Font &btn_font) : font(btn_font){
        // TEXT
        text_size = size.x/4.f;
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
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});

        sf::FloatRect spriteBounds = background.getGlobalBounds();
        text.setPosition({spriteBounds.position.x + spriteBounds.size.x / 2.f,
                          spriteBounds.position.y + spriteBounds.size.y / 2.f - 2});

        //
        state = State::Normal;
    }

    ~Button() override = default; // idc

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
            background.setTexture(textures.at(state).getTexture());
        } catch (std::exception& e){
            background.setTexture(textures.at(State::Normal).getTexture());
        }

    }

    void render(sf::RenderTarget &target) override {
        target.draw(background);
        target.draw(text);
    }

    void set_position(sf::Vector2f _pos) override {
        pos = _pos;
        bounds.position = _pos;
    }

    void set_size(sf::Vector2f _size) override {
        size = _size;
        bounds.size = _size;
    }

    bool load_texture(const std::string& path, State _state){
        sf::Texture texture;
        if (!texture.loadFromFile(path)) {
            return false;
        }
        sf::Vector2u textureSize = texture.getSize();
        sf::Vector2f buttonSize = bounds.size;

        sf::Sprite sprite(texture);
        sprite.setScale({
            buttonSize.x / static_cast<float>(textureSize.x),
            buttonSize.y / static_cast<float>(textureSize.y)
        });

        if (!textures[_state].resize(static_cast<sf::Vector2u>(buttonSize))) {
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
        on_click = func;
    }

    void set_on_release(std::function<void()> func) {
        on_release = func;
    }

    void set_string(const std::string &_text){
        text.setString(_text);
    }
private:
    enum class Type{
        Undefined,
        Simple,
        Breakpoint,
        Textured
    }; // maybe useless [[0_0]]

    State state = State::Undefined;
    Type type   = Type::Undefined;

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
