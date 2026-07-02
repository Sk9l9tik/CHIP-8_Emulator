#include <cstdio>

#include <iostream>

#include "core/CHIP_8.h"

#define CHIP8_DEBUG1

#ifdef CHIP8_DEBUG
#include "debug/Debugger.h"
#endif

int main(){
    CHIP_8 emulator {};
    emulator.load_ROM("IBM_Logo.ch8"); //укажите здесь пути до этого прикола свои

#ifdef CHIP8_DEBUG
    Debugger d(emulator);
    
    d.set_breakpoint(0x200);

    while(!d.is_paused())
      d.update();
    std::cout << "Hit breakpoint at PC = 0x" << std::hex << emulator.get_cpu().get_PC() << "\n";
#endif //DEBUG

    std::cout << "Font Check\n";
    for(int i = 0; i < 5; ++i)
        printf("%04X: %02X\n", 0x050 + i, emulator.get_memory().raw()[0x050+i]);

    for(int i = 0; i < 16; ++i)
        printf("%04X: %02X\n", 0x200+i, emulator.get_memory().raw()[0x200+i]);
    emulator.get_cpu().tick();
    emulator.get_cpu().tick(); //вторая инструкция из этого РОМа пока не реализована если чё
    return 0;
}
