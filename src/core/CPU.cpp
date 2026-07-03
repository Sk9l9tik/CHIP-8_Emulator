#include <iostream>
#include <stdexcept>
#include <sstream>
#include <random>

#include "core/CPU.h"


void CPU::print() {std::cout << "Hello World!";}
uint16_t CPU::fetch() {
    uint16_t opcode = (memory.read(program_counter) << 8u) | memory.read(program_counter+1);
    program_counter += 2;
    return opcode;
}
void CPU::execute(uint16_t opcode) {
    /*Все инструкции имеют шаблон
     * CXYN, где C - тип инструкции; X, Y - регистры операндов; N - константа
     * CXNN, где NN - константа 8 бит
     * CNNN, где NNN - адрес 12 бит*/
    uint8_t C = (opcode & 0xF000) >> 12;
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;
    uint8_t NN = opcode & 0x00FF;
    uint16_t NNN = opcode & 0x0FFF;

    auto throw_unknown = [&](){
        std::ostringstream oss;
        oss << "Unknown instruction passed at line: 0x" << std::hex << std::uppercase << program_counter - 2;
        throw std::runtime_error(oss.str());
    };

    switch(C){
        case 0x0:
            switch(NN){
                case 0xE0: //CLS (Очищение экрана)
                    frame_buffer.clear_screen();
                    break;
                case 0xEE: //Ret (Возврат из подпроги)
                    --stack_pointer;
                    program_counter = stack.at(stack_pointer);
                    break;
                default: throw_unknown();
            }
            break;
        case 0x1: //JMP NNN
            program_counter = NNN;
            break;
        case 0x2: //CALL NNN
            stack[stack_pointer] = program_counter;
            ++stack_pointer;
            program_counter = NNN;
            break;
        case 0x3: //SE Vx, NN (Пропуск след. инструкции, если Vx == NN)
            if(V.at(X) == NN) program_counter += 2;
            break;
        case 0x4: //SNE Vx, NN (Пропуск след. инструкции, если Vx != NN)
            if(V.at(X) != NN) program_counter += 2;
            break;
        case 0x5: //SE Vx, Vy (Пропуск след. инструкции, если Vx == Vy)
            if(V.at(X) == V.at(Y)) program_counter += 2;
            break;
        case 0x6: //LD Vx, NN (Загрузить в Vx NN)
            V.at(X) = NN;
            break;
        case 0x7: //ADD Vx, NN (Добавить к Vx NN)
            V.at(X) += NN;
            break;
        case 0x8:
            switch(N){
                case 0x0: //LD Vx, Vy (Установить Vx равным Vy)
                    V.at(X) = V.at(Y);
                    break;
                case 0x1: //OR Vx, Vy (Побитово сложить Vx, Vy, результат в Vx)
                    V.at(X) |= V.at(Y);
                    break;
                case 0x2: //AND Vx, Vy (Побитово умножить Vx, Vy, результат в Vx)
                    V.at(X) &= V.at(Y);
                    break;
                case 0x3: //XOR Vx, Vy (Сложение по модулю 2 Vx, Vy, результат в Vx)
                    V.at(X) ^= V.at(Y);
                    break;
                case 0x4: //ADD Vx, Vy (Сложение Vx, Vy; Если сумма больше 255, то установка регистра флагов)
                {
                    uint16_t sum = V.at(X) + V.at(Y);
                    V.at(0xF) = (sum > 0xFF) ? 1 : 0;
                    V.at(X) = sum & 0xFF;
                    break;
                }
                case 0x5: //SUB Vx, Vy (Вычитание Vx, Vy; Если Vx > Vy, то установка регистра флагов)
                    V.at(0xF) = (V.at(X) > V.at(Y)) ? 1 : 0;
                    V.at(X) -= V.at(Y);
                    break;
                case 0x6: //SHR Vx (Сдвиг Vx вправо на 1; Если число нечётное, то установка регистра флагов)
                    V.at(0xF) = V.at(X) & 0x1;
                    V.at(X) >>= 1;
                    break;
                case 0x7: //SUBN Vx, Vy (Вычитание Vy, Vx; Если Vy > Vx, то установка регистра флагов. Результат всё равно в Vx)
                    V.at(0xF) = (V.at(Y) > V.at(X)) ? 1: 0;
                    V.at(X) = V.at(Y) - V.at(X);
                    break;
                case 0xE: //SHL Vx (Сдвиг Vx вправо на 1; Если старший бит 1, то установка регистра флагов)
                    V.at(0xF) = (V.at(X) & 0x80) >> 7u;
                    V.at(X) <<= 1;
                    break;
                default:
                    throw_unknown();
            }
            break;
        case 0x9: //SNE Vx, Vy (Пропуск след. инструкции, если Vx != Vy)
            if(V.at(X) != V.at(Y)) program_counter += 2;
            break;
        case 0xA: //LD I, NNN (Установить регистр индекса в NNN)
            index = NNN;
            break;
        case 0xB: //JMP V0, NNN (Скачок на адрес NNN + V0)
            program_counter = V.at(0) + NNN;
            break;
        case 0xC: //RND Vx, NN (Установить Vx в рандомное значение побитово умноженное на NN)
            V.at(X) = ([](){
                static std::mt19937 rng{std::random_device{}()};
                static std::uniform_int_distribution<uint8_t> dist(0, 255);
                return dist(rng);
            }()) & NN;
            break;
        case 0xD: //DRW Vx, Vy, nibble (Рисует n-байт спрайт начиная с index в точке (Vx, Vy). Если были коллизии, VF=1
        {
            uint8_t x_pos = V.at(X) % FrameBuffer::WIDTH;
            uint8_t y_pos = V.at(Y) % FrameBuffer::HEIGHT;
            const uint8_t* sprite = memory.raw() + index;
            V.at(0xF) = frame_buffer.draw_sprite(x_pos, y_pos, sprite, N) ? 1 : 0;
            break;
        }
        case 0xE:
            switch(NN){
                case 0x9E: //SKP Vx (Пропуск след. инструкции, если клавиша номер Vx нажата)
                    if(keypad.at(V.at(X))) program_counter += 2;
                    break;
                case 0xA1: //SKNP Vx (обратно прошлой, если не нажата)
                    if(!keypad.at(V.at(X))) program_counter += 2;
                    break;
                default: throw_unknown();
            }
            break;
        case 0xF:
            switch(NN){
                case 0x07: //LD Vx, DT (Установить значение из таймера задержки в Vx)
                    V.at(X) = delay_timer;
                    break;
                case 0x0A: //LD Vx, K (Дождаться нажатия, сохранить значение клавиши в Vx)
                    waiting_for_key = true;
                    key_register = X;
                    break;
                case 0x15: //LD DT, Vx (Установить таймер задержки равным Vx)
                    delay_timer = V.at(X);
                    break;
                case 0x18: //LD ST, Vx (Аналогично для звукового таймера)
                    sound_timer = V.at(X);
                    break;
                case 0x1E: //ADD I, Vx (Установить регистр индекса равным index + Vx)
                    index += V.at(X);
                    break;
                case 0x29: //LD F, Vx (Установить регистр индекса равным месту в памяти, гре хранится символ Vx)
                    index = Memory::FONT_START + (5 * V.at(X));
                    break;
                case 0x33: // LD B, Vx (Положить в память двоично-десятичное представление Vx)
                    memory.write(index, V.at(X) / 100); //сотни
                    memory.write(index + 1, (V.at(X) / 10) % 10); //десятки
                    memory.write(index + 2, V.at(X) % 10); //единицы
                    break;
                case 0x55: //LD [I], Vx (Положить в память регистры от V0 до Vx начиная с I (индекса))
                    for(uint8_t i = 0; i <= X; ++i)
                        memory.write(index + i, V.at(i));
                    break;
                case 0x65: //LD Vx, [I] (Прочитать из памяти значения X раз начиная с индекса и записать в регистры от V0 до Vx)
                    for(uint8_t i = 0; i <= X; ++i)
                        V.at(i) = memory.read(index + i);
                    break;
                default: throw_unknown();
            }
            break;
        default:
            throw_unknown();
    }
}
CPU::CPU(Memory &memory_, FrameBuffer &frame_buffer_): memory(memory_), frame_buffer(frame_buffer_) {
    reset();
}
void CPU::reset() {
    V.fill(0);
    index = 0;
    program_counter = Memory::ROM_START;
    stack.fill(0);
    stack_pointer = 0;
    delay_timer = 0;
    sound_timer = 0;
    keypad.fill(0);
    waiting_for_key = false;
    key_register = 0;
    frame_buffer.clear_screen();
}
void CPU::tick() {
    if(waiting_for_key) [[unlikely]] return;
    uint16_t opcode = fetch();
    execute(opcode);
}
void CPU::set_key_state(uint8_t key, bool pressed) {
    if(key > 0xF) return;
    keypad.at(key) = pressed;
    if(waiting_for_key && pressed){
        V.at(key_register) = key;
        waiting_for_key = false;
    }
}
bool CPU::get_key_state(uint8_t key) const { return keypad.at(key);}
void CPU::set_sound_timer(uint8_t value) noexcept {
    sound_timer = value;
}
void CPU::set_delay_timer(uint8_t value) noexcept {
    delay_timer = value;
}
uint8_t CPU::get_delay_timer() const noexcept { return delay_timer;}
uint8_t CPU::get_sound_timer() const noexcept { return sound_timer;}
void CPU::tick_timers() {
    if(delay_timer > 0) delay_timer--;
    if(sound_timer > 0) sound_timer--;
}
uint16_t CPU::get_PC() const noexcept { return program_counter; }
uint8_t CPU::get_register(uint8_t reg_index) const { return V.at(reg_index); }
uint16_t CPU::get_index() const noexcept { return index;}
uint8_t CPU::get_stack_pointer() const noexcept { return stack_pointer;}
uint16_t CPU::get_stack(uint8_t stack_index) const { return stack.at(stack_index);}
