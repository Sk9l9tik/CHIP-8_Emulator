#include <iostream>
#include "App.h"


int main(int argc, char* argv[]){
    std::string rom_path{(argc >= 2) ? argv[1] : "../assets/roms/Tetris_.ch8"};


    try{
        App app(rom_path);
        app.run();
    } catch(const std::exception& e){
        std::cerr << "Fatal  error: " << e.what() << '\n';
        return -1;
    }
    return 0;
}
