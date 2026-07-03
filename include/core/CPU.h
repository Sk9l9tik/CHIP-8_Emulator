#ifndef CHIP_8_EMULATOR_CPU_H
#define CHIP_8_EMULATOR_CPU_H

#include <cstdint>
#include <array>

#include "core/Memory.h"
#include "core/FrameBuffer.h"


class CPU{
private:
    Memory& memory;
    FrameBuffer& frame_buffer;

    std::array<uint8_t,16> V{}; //регистры
    uint16_t index{};
    uint16_t program_counter{};
    std::array<uint16_t, 16> stack{};
    uint8_t stack_pointer{};
    uint8_t delay_timer{};
    uint8_t sound_timer{};
    std::array<uint8_t, 16> keypad{};
    bool waiting_for_key{};
    uint8_t key_register{};

    uint16_t fetch();
    void execute(uint16_t opcode);
public:
    explicit CPU(Memory& memory_, FrameBuffer& frame_buffer_);

    void reset();
    void tick(); // Выполнить одну инструкцию
    [[deprecated("Deprecated test function")]] void print();

    //Для звука/графики (НАВЕРНО)
    void set_key_state(uint8_t key, bool state);
    bool get_key_state(uint8_t key) const;
    void set_sound_timer(uint8_t value) noexcept;
    void set_delay_timer(uint8_t value) noexcept;
    [[nodiscard]] uint8_t get_delay_timer() const noexcept;
    [[nodiscard]] uint8_t get_sound_timer() const noexcept;
    void tick_timers();

    //Для дебага
    [[nodiscard]] uint16_t get_PC() const noexcept;
    [[nodiscard]] uint8_t get_register(uint8_t index) const;
    [[nodiscard]] uint16_t get_index() const noexcept;
    [[nodiscard]] uint8_t get_stack_pointer() const noexcept;
    [[nodiscard]] uint16_t get_stack(uint8_t index) const;
    //uint8_t get_delay_timer() const;
};
#endif //CHIP_8_EMULATOR_CPU_H
