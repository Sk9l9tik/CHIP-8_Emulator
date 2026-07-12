#ifndef CHIP_8_EMULATOR_TABLE_H
#define CHIP_8_EMULATOR_TABLE_H

#include "ui/Widget.h"

class Table : public Widget{
public:
    Table(uint32_t cols, uint32_t rows) : cols_c(cols), rows_c(rows){
        _table.resize(cols*rows);

        background.setSize(size);
        set_bg_color(0x333333FF);
    }

    bool add_widget(Widget* widget) {
        if(!widget) return false;

        for(uint32_t i = 0; i < _table.size(); i++){
            if(_table[i] == nullptr){
                return add_widget(widget, i % cols_c, i / rows_c);
            }
        }

        return false;
    }

    bool add_widget(Widget* widget, uint32_t col, uint32_t row, bool rewrite = true) {
        uint32_t index = col + row * cols_c;
        if(!widget || index > _table.size() || (!rewrite && _table[index] != nullptr) ||
           col >= cols_c || row >= rows_c){
            return false;
        }

        widget->set_parent(this);
        _table[index] = widget;

        recount_size();
        replace_widgets();

        return true;
    } // Set widget to [x+y*cols] cell

    uint32_t get_cols_c() const {
        return cols_c;
    }

    uint32_t get_rows_c() const {
        return rows_c;
    }

    void handle_event(const std::optional<sf::Event> &event) override {
        for(auto& el: _table){
            if(el != nullptr)
                el->handle_event(event);
        }
    }

    void update() override {
        if(background.getSize() != size) background.setSize(size);

        for(auto& el: _table){
            if(el != nullptr)
                el->update();
        }
    }

    void render(sf::RenderTarget &target) override {
        if(background.getSize().x != 0 && background.getSize().y != 0) {
            sf::RenderTexture bg_texture(static_cast<sf::Vector2u>(size));
            bg_texture.clear(sf::Color::Transparent);
            bg_texture.draw(background);
            bg_texture.display();

            sf::Sprite bg_sprite(bg_texture.getTexture());
            bg_sprite.setPosition(pos);

            target.draw(bg_sprite);
        }
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

    void set_bg_color(uint32_t color){
        background.setFillColor(static_cast<sf::Color>(color));
    }
private:
    void recount_size(){
        float max_wx = 0.f, max_wy = 0.f;

        for(auto pw : _table){
            if(pw){
                max_wx = std::max(max_wx, pw->get_size().x+pw->style.padding.left+pw->style.padding.right);
                max_wy = std::max(max_wy, pw->get_size().y+pw->style.padding.top+pw->style.padding.bottom);
            }
        }
        cell_size = {max_wx, max_wy};

        float new_tx = max_wx * cols_c
                       + style.padding.left + style.padding.right
                       + style.gap.horizontal*(cols_c - 1);

        float new_ty = max_wy * rows_c
                       + style.padding.top + style.padding.bottom
                       + style.gap.vertical*(rows_c - 1);

        size = {new_tx, new_ty };
    }

//    void replace_widgets(){
//        float x = style.padding.left;
//        float y = style.padding.top;
//
//        for(uint32_t j = 0; j < rows_c; j++){
//            x = style.padding.left;
//            for(uint32_t i = 0; i < cols_c; i++){
//                auto pw = _table[i + j*cols_c];
//                if(pw){
//                    pw->set_position({
//                        x + style.gap.horizontal * (i%cols_c != 0) + pw->style.padding.left,
//                        y + style.gap.vertical * (j != 0) + pw->style.padding.top
//                    });
//
//                }
//                x += cell_size.x;
//            }
//            y += cell_size.y;
//        }
//    }
    void replace_widgets(){
        float x;
        float y = pos.y + style.padding.top;

        for(uint32_t j = 0; j < rows_c; j++){
            x = pos.x + style.padding.left;
            for(uint32_t i = 0; i < cols_c; i++){
                auto pw = _table[i + j*cols_c];
                if(pw){
                    pw->set_position({
                                             x + pw->style.padding.left,
                                             y + pw->style.padding.top
                                     });
                }
                x += cell_size.x + style.gap.horizontal;
            }
            y += cell_size.y + style.gap.vertical;
        }
    }
    sf::RectangleShape background;
    uint32_t cols_c;
    uint32_t rows_c;

    sf::Vector2f cell_size = {0.f, 0.f};

    std::vector<Widget*> _table;
};

#endif //CHIP_8_EMULATOR_TABLE_H
