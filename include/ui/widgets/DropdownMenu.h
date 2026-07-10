#ifndef CHIP_8_EMULATOR_LIST_H
#define CHIP_8_EMULATOR_LIST_H

#include "ui/Widget.h"
#include "ui/widgets/Button.h"

// UNDER CONSTRUCTION

class DropdownMenu : public Widget{
public:
    DropdownMenu(std::string name, sf::Font font)
    : open_list(name, {180, 40}, {0,0}, font)
    {
        open_list.set_on_mouse_entered([this](){
            this->shrink();
        });

        open_list.set_on_mouse_exited([this](){
            this->expand();
        });
    };

    void expand(){  expanded = true;  }
    void shrink(){  expanded = false; }

    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i &mouse_pos) override {
        open_list.handle_event(event, mouse_pos);
        if(expanded){

        }
    }

    void update() override {
        open_list.update();
        if(expanded){

        }
    }

    void render(sf::RenderTarget &target) override {
        open_list.render(target);
        if(expanded){

        }
    }

    void add_widget(Widget* widget){
        if(!widget) return;

        elements.push_back(widget);
        widget->set_parent(this);
        //widget->set_position
    }

private:
    bool expanded = false;

    Button open_list;
    std::vector<Widget*> elements;
};

#endif //CHIP_8_EMULATOR_LIST_H
