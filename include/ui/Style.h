#ifndef CHIP_8_EMULATOR_STYLE_H
#define CHIP_8_EMULATOR_STYLE_H

class Style{
public:
    struct Padding {
        float top;
        float right;
        float bottom;
        float left;

        Padding(float u, float r, float d, float l) : top(u), right(r), bottom(d), left(l) {};
        Padding(float x) : top(x), right(x), bottom(x), left(x) {}
    } padding {0};

    // Table only atp
    struct Gap{
        float horizontal = 0.f;
        float vertical   = 0.f;

        Gap(float h, float v) : horizontal(h), vertical(v) {}
        Gap(float x) : horizontal(x), vertical(x) {}
    } gap {0};
};

#endif //CHIP_8_EMULATOR_STYLE_H
