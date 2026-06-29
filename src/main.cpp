#include "CPU.h"
#include "Memory.h"
#include <iostream>

int main(){
    Memory memory{};
    CPU cpu {memory};
    std::cout << "SAERMO::" << static_cast<int>(cpu.get_stack_pointer()) << '\n';
    return 0;
}