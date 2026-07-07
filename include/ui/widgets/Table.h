#ifndef CHIP_8_EMULATOR_TABLE_H
#define CHIP_8_EMULATOR_TABLE_H

#include "ui/Widget.h"

//


class Table : public Widget{
public:
    //Style
    float vertical_gap = 0.f;
    float horizontal_gap = 0.f;
    //

    Table(uint32_t cols, uint32_t rows) : cols_c(cols), rows_c(rows){
        _table.resize(cols*rows);
    }

    bool add_widget(Widget* widget) {
        float x = pos.x + padding.left;
        float y = pos.y + padding.up;

        float pd = 0.f;
        for(uint32_t j = 0; j < rows_c; j++){
            for(uint32_t i = 0; i < cols_c; i++){
                uint32_t index = i + j * cols_c;
                if(_table[index] == nullptr){
                    _table[index] = widget;
                    widget->set_position({x, y});
                    widget->set_parent(this);
                    return true;
                }
                x += _table[index]->get_size().x + horizontal_gap + _table[index]->padding.right;

                pd = std::max(_table[index]->padding.down, pd);
            }
            x = pos.x + padding.left;

            y += _table[j * cols_c]->get_size().y + vertical_gap + pd;
        }
        return false;
    }

//    bool add_widget(Widget* widget, uint32_t x, uint32_t y, bool rewrite = true) {
//        return false;
//    }; // Set widget to [x+y*cols] cell

    uint32_t get_cols_c() const {
        return cols_c;
    }

    uint32_t get_rows_c() const {
        return rows_c;
    }

    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i &mouse_pos) override {
        for(auto& el: _table){
            if(el != nullptr)
                el->handle_event(event, mouse_pos);
        }
    }

    void update() override {
        for(auto& el: _table){
            if(el != nullptr)
                el->update();
        }
    }

    void render(sf::RenderTarget &target) override {
        for (auto& el : _table) {
            if(el != nullptr)
                el->render(target);
        }
    }

    void clear() {
        cols_c = 0;
        rows_c = 0;
        _table.clear(); // *...
    };

private:
    uint32_t cols_c;
    uint32_t rows_c;

    std::vector<Widget*> _table;
};

#endif //CHIP_8_EMULATOR_TABLE_H
