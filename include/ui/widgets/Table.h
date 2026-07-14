#ifndef CHIP_8_EMULATOR_TABLE_H
#define CHIP_8_EMULATOR_TABLE_H

#include <numeric>
#include "ui/Widget.h"

class Table : public Widget{
public:
    Table(uint32_t cols, uint32_t rows) : cols_c(cols), rows_c(rows){
        _table.resize(cols*rows);

        row_sizes.resize(rows);
        col_sizes.resize(cols);

        background.setSize(size);
        set_bg_color(0x313244FF);
    }

    bool add_widget(Widget* widget) {
        if(!widget) return false;

        for(uint32_t i = 0; i < _table.size(); i++){
            if(_table[i] == nullptr){
                return add_widget(widget, i % cols_c, i / cols_c);
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
        recount_size();
        replace_widgets();
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

    void set_bg_color(sf::Color color){
        background.setFillColor(color);
    }
private:
    void recount_size(){
        for(uint32_t j = 0; j < rows_c; j++){
            for(uint32_t i = 0; i < cols_c; i++) {
                auto pw = _table[i+j*cols_c];
                if (pw) {
//                    col_sizes[i] = std::max(col_sizes[i],
//                                            pw->get_size().x + pw->style.padding.top + pw->style.padding.bottom);
//                    row_sizes[j] = std::max(row_sizes[j],
//                                            pw->get_size().y + pw->style.padding.left + pw->style.padding.right);
                    col_sizes[i] = std::max(col_sizes[i],
                                            pw->get_size().x + pw->style.margin.left + pw->style.margin.right);
                    row_sizes[j] = std::max(row_sizes[j],
                                            pw->get_size().y + pw->style.margin.top + pw->style.margin.bottom);
                }
            }
        }

        float new_tx = std::accumulate(col_sizes.begin(),col_sizes.end(), 0.f)
                       + style.margin.left + style.margin.right
                       + style.gap.horizontal*static_cast<float>(cols_c - 1);

        float new_ty = std::accumulate(row_sizes.begin(),row_sizes.end(), 0.f)
                       + style.margin.top + style.margin.bottom
                       + style.gap.vertical*static_cast<float>(rows_c - 1);

        size = {new_tx, new_ty };
    }

    void replace_widgets(){
        float y = style.margin.top + pos.y;

        for(uint32_t j = 0; j < rows_c; j++){
            float x = style.margin.left + pos.x;

            for(uint32_t i = 0; i < cols_c; i++){
                auto pw = _table[i + j * cols_c];
                if(pw){
                    float cell_x = x + pw->style.margin.left;
                    float cell_y = y + pw->style.margin.top;

                    pw->set_position({
                        cell_x,
                        cell_y
                    });
                }
                x += col_sizes[i] + style.gap.horizontal;
            }
            y += row_sizes[j] + style.gap.vertical;
        }
    }

    sf::RectangleShape background;
    uint32_t cols_c;
    uint32_t rows_c;

    std::vector<float> row_sizes;
    std::vector<float> col_sizes;

    std::vector<Widget*> _table;
};

#endif //CHIP_8_EMULATOR_TABLE_H
