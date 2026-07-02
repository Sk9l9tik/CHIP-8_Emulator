#include <iostream>
#include <stdexcept>
#include <sstream>

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

    std::ostringstream oss;
    oss << "Unknown instruction passed at line: 0x" << std::hex << std::uppercase; //TODO: переписать под лямбда-функцию

    switch(C){
        case 0x0:
            if(opcode == 0x00E0) //очищение экрана
                frame_buffer.clear_screen();
            else if(opcode == 0x00EE){ //возврат из подпроги RET
                --stack_pointer;
                program_counter = stack[stack_pointer];
            }
            else{
                oss << (program_counter-2);
                throw std::runtime_error(oss.str());
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
            }
            break;
        default:
            oss << (program_counter-2);
            throw std::runtime_error(oss.str());
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
    uint16_t opcode = fetch();
    execute(opcode);
}
void CPU::set_key_state(uint8_t key, bool pressed) {}
bool CPU::get_key_state(uint8_t key) const { return 0;}
void CPU::set_sound_timer(uint8_t value) {
    sound_timer = value;
}
void CPU::set_delay_timer(uint8_t value) {
    delay_timer = value;
}
uint8_t CPU::get_delay_timer() const { return delay_timer;}
void CPU::tick_timers() {
    if(delay_timer > 0) delay_timer--;
    if(sound_timer > 0) sound_timer--;
}
uint16_t CPU::get_PC() const { return program_counter; }
uint8_t CPU::get_register(uint8_t reg_index) const { return V.at(reg_index); }
uint16_t CPU::get_index() const { return index;}
uint8_t CPU::get_stack_pointer() const { return stack_pointer;}
uint16_t CPU::get_stack(uint8_t stack_index) const { return stack.at(stack_index);}
