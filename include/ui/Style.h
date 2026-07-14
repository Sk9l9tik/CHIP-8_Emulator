#ifndef CHIP_8_EMULATOR_STYLE_H
#define CHIP_8_EMULATOR_STYLE_H

class Style{
public:
    struct Padding {
        float top;
        float right;
        float bottom;
        float left;

        Padding(float t, float r, float b, float l) : top(t), right(r), bottom(b), left(l) {}
        Padding(float h, float v) : top(v), right(h), bottom(v), left(h) {}
        Padding(float x) : top(x), right(x), bottom(x), left(x) {}
    } padding {0};

    struct Gap{
        float horizontal = 0.f;
        float vertical   = 0.f;

        Gap(float h, float v) : horizontal(h), vertical(v) {}
        Gap(float x) : horizontal(x), vertical(x) {}
    } gap {0};
};

#endif //CHIP_8_EMULATOR_STYLE_H
