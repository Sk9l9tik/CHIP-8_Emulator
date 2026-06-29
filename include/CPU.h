#ifndef CHIP_8_EMULATOR_CPU_H
#define CHIP_8_EMULATOR_CPU_H

#include <cstdint>
#include "Memory.h"

class CPU{
private:
    Memory& memory;

    uint8_t V[16]{}; //регистры
    uint16_t index{};
    uint16_t program_counter{};
    uint16_t stack[16]{};
    uint8_t stack_pointer{};
    uint8_t delay_timer{};
    uint8_t sound_timer{};
    uint8_t keypad[16]{};
    bool waiting_for_key{};
    uint8_t key_register{};

    uint16_t fetch();
    void execute(uint16_t opcode);
public:
    CPU(Memory& memory_): memory(memory_){}

    void reset();
    void tick(); // Выполнить одну инструкцию
    [[deprecated("Была тестовой функцией")]] void print();

    //Для звука/графики (НАВЕРНО)
    void set_key_state(uint8_t key, bool state);
    bool get_key_state(uint8_t key) const;
    uint8_t set_sound_timer(uint8_t value) const;
    uint8_t set_delay_timer(uint8_t value) const;
    uint8_t get_delay_timer() const;
    void tick_timers();

    //Для дебага
    uint16_t get_PC() const;
    uint8_t get_register(uint8_t index) const;
    uint16_t get_index() const;
    uint8_t get_stack_pointer() const;
    uint16_t get_stack(uint8_t index) const;
    //uint8_t get_delay_timer() const;
};
#endif //CHIP_8_EMULATOR_CPU_H
