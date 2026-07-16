#ifndef CHIP_8_CPU_VIEWER_H
#define CHIP_8_CPU_VIEWER_H

#include <cstdint>

#include "core/CPU.h"


struct CPUState {
    std::array<uint8_t,16> V{};

    uint16_t PC{};
    uint16_t I{};

    std::array<uint16_t, 16> stack{};
    uint8_t SP{};

    uint8_t DT;
    uint8_t ST;

    std::array<uint8_t, 16> keypad;
};

class CPUViewer {
public:

  [[nodiscard]] static CPUState snapshot(const CPU& cpu) noexcept; 

};

#endif
