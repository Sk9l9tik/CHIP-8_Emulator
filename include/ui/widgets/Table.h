#ifndef CHIP_8_EMULATOR_TABLE_H
#define CHIP_8_EMULATOR_TABLE_H

#include "ui/Widget.h"

//

class Table : Widget{
public:
    bool add_widget(); // Add to first empty cell
    bool add_widget(uint32_t x, uint32_t y, bool rewrite = true); // Add to [x+y*cols] cell

    uint32_t get_cols_c() const {
        return cols_c;
    }

    uint32_t get_rows_c() const {
        return rows_c;
    }

    void handle_event(const std::optional<sf::Event> &event, const sf::Vector2i &mouse_pos) override {}
    void update() override {}
    void render(sf::RenderTarget &target) override {}

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
