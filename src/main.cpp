#include "CHIP_8.h"
#include <iostream>

int main(){
    CHIP_8 emulator {};
    std::cout << "SAERMO::" << static_cast<int>(emulator.get_cpu().get_stack_pointer()) << '\n';
    return 0;
}