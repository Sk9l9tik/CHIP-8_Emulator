# CHIP-8 Emulator

## RU:

Учебный проект - эмулятор ВМ CHIP-8.

CHIP-8 - интерпретируемый язык, использовавшийся в середине 1970-х годов для упрощения разработки игр на 8-битных микрокомпьютерах. Несмотря на простоту, его архитектура включает все базовые концепции реальных процессоров: регистры, адресуемую память, стек вызовов, цикл выборки-декодировании-исполнения инструкций.
### Возможности
* Полная поддержка системы команд CHIP-8
* Графический интерфейс на SFML 3
* Встроенный отладчик: пошаговое выполнение, точки останова, дизассемблер, просмотр памяти
* Юнит-тестирование
* Загрузка произвольных ROM-файлов
### Архитектура
Проект разделён на три модуля, разрабатываемых параллельно:
* core/ - ядро эмулятора
* ui/ - графический интерфейс
* debug/ - отладчик
#### core/ - ядро эмулятора

| Класс       | Ответственность                                                                                |
|-------------|------------------------------------------------------------------------------------------------|
| Memory      | Хранение 4 КБ адресного пространства, загрузка шрифтов и ROM, чтение/запись с проверкой границ |
| FrameBuffer | Буфер кадра 64x32, отрисовка спрайтов, детекция коллизий                                       |
| CPU         | Регистры, стек, таймеры, цикл fetch-decode-execute, обработка всех опкодов                     |
| CHIP_8      | Оркестратор - владеет Memory/FrameBuffer/CPU, следит за порядком их инициализации              |

*CPU* не владеет *Memory* и *FrameBuffer*, а хранит ссылки на них - это позволяет модулям графики и отладки работать с теми же объектами, которые исполняет процессор без дублирования состояния.

Расположение данных в памяти

0x000-0x04F не используется
0x050-0x09F встроенный шрифтовой набор
0x0A0-0x1FF не используется
0x200-0xFFF ROM-код программы

Буфер кадра **не** хранится в основной памяти (в отличие от оригинальной COSMAC VIP, где под дисплей была зарезервирована зона 0xF00-0xFFF) - это снижает риск случайной порчи видеопамяти опкодами вроде FX55.

#### ui/ - графический интерфейс

| Класс | Ответственность |
|-------|-----------------|
| TODO  |                 |

#### debug/ - отладчик

| Класс | Ответственность |
|-------|-----------------|
| TODO  |                 |

### Установка
**Ожидается скорый релиз**

### Требования для самостоятельной сборки
* Компилятор с поддержкой C++23
* CMake 3.31+

Git
  Зависимости подтягиваются автоматически через FetchContent, если не найдены в системе:
* SFML 3.0.2
* GoogleTest 1.17.0
### Сборка
```bash
git clone https://github.com/Sk9l9tik/CHIP-8_Emulator.git
cd CHIP-8_Emulator
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
### Запуск
```bash
./chip8_emu <путь до ROM-файла>
```
Пример (Windows)
```bash
cd cmake-build-release
```
```bash
./chip8_emu.exe Tetris_.ch8
```


В эмуляторе также есть кнопка открытия ROM, если эмулятор уже работает.
### Управление
| Оригинал        | Эмулятор        |
|-----------------|-----------------|
| `1` `2` `3` `C` | `2` `3` `4` `Z` |
| `4` `5` `6` `D` | `Q` `W` `E` `X` |
| `7` `8` `9` `E` | `R` `A` `S` `C` |
| `A` `0` `B` `F` | `D` `1` `F` `V` |
## EN:
The educational project - CHIP-8 VM emulator.

CHIP-8 is an interpreted language used in the mid-1970s to simplify game development on 8-bit microcomputers. Despite its simplicity, its architecture includes all the basic concepts of real processors: registers, addressable memory, a call stack, and the fetch-decode-execute cycle of instructions.
### Features
* Full support for the CHIP-8 command system
* Graphical interface based on SFML 3
* Built-in debugger: step-by-step execution, breakpoints, disassembler
* Unit testing
* Loading custom ROM files
### Architecture
The project is divided into three modules that are being developed in parallel:
* core/ - the core of the emulator
* ui/ - graphical interface
* debug/ - debugger
#### core/ - the core of the emulator

| Class       | Responsibility                                                                               |
|-------------|----------------------------------------------------------------------------------------------|
| Memory      | Storing 4 KB of address space, loading fonts and ROM, read/write with bounds checking        |
| FrameBuffer | 64x32 frame buffer, sprite rendering, collision detection                                    |
| CPU         | Registers, stack, timers, fetch-decode-execute cycle, processing of all opcodes              |
| CHIP_8      | The orchestrator owns the Memory/FrameBuffer/CPU, monitors the order of their initialization |

The *CPU* does not own *Memory* and *FrameBuffer*, but stores references to them. This allows the graphics and debugging modules to work with the same objects executed by the processor without duplicating the state.

Location of data in memory

0x000-0x04F not used
0x050-0x09F built-in font set
0x0A0-0x1FF is not used
0x200-0xFFF ROM program code

The frame buffer **is not** stored in the main memory (unlike the original COSMAC VIP, where the 0xF00-0xFFF zone was reserved for the display) - this reduces the risk of accidental damage to video memory by opcodes like FX55.

#### ui/ - graphical interface

| Class | Responsibility |
|-------|----------------|
| TODO  |                |

#### debug/ - debugger

| Class | Responsibility |
|-------|----------------|
| TODO  |                |

### Installation
**Release coming soon**

### Requirements for building
* Compiler with C++23
* CMake 3.31+

Git
  Dependencies are pulled up automatically via fetchContent if they are not found in the system:
* SFML 3.0.2
* GoogleTest 1.17.0
### Building
```bash
git clone https://github.com/Sk9l9tik/CHIP-8_Emulator.git
cd CHIP-8_Emulator
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
### Launching
```bash
./chip8_emu <path-to-ROM>
```
Example (Windows)
```bash
cd cmake-build-release
```
```bash
./chip8_emu.exe Tetris_.ch8
```
The emulator also has a button to open the ROM if the emulator is already running.
### Controls

| Original        | Emulator        |
|-----------------|-----------------|
| `1` `2` `3` `C` | `2` `3` `4` `Z` |
| `4` `5` `6` `D` | `Q` `W` `E` `X` |
| `7` `8` `9` `E` | `R` `A` `S` `C` |
| `A` `0` `B` `F` | `D` `1` `F` `V` |

### Авторы (Credits):

| IMG                                                                      | Участник (Contributor)                   | Роль (Role)            |
|--------------------------------------------------------------------------|------------------------------------------|------------------------|
| <img src="https://github.com/Sk9l9tik.png" width="50" height="50">       | [Sk9l9tik](https://github.com/Sk9l9tik)  | Debugger               |
| <img src="https://github.com/crw884.png" width="50" height="50">         | [crw884](https://github.com/crw884)      | UI, SFML, Design       |
| <img src="https://github.com/SosikuKawiiHog.png" width="50" height="50"> | [SKH](https://github.com/SosikuKawiiHog) | Core, Module Contracts |