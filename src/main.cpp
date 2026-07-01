#include "CHIP_8.h"
#include <iostream>
#include <cstdio>

int main(){
    CHIP_8 emulator {};
    emulator.load_ROM("IBM Logo.ch8"); //укажите здесь пути до этого прикола свои
    std::cout << "Font Check\n";
    for(int i = 0; i < 5; ++i){
        printf("%04X: %02X\n", 0x050 + i, emulator.get_memory().raw()[0x050+i]);
    }

    for(int i = 0; i < 16; ++i)
        printf("%04X: %02X\n", 0x200+i, emulator.get_memory().raw()[0x200+i]);
    return 0;
}