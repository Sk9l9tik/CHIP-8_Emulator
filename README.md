# CHIP-8 Emulator

[🇬🇧 English](README.md) | [🇷🇺 Русский](README_RU.md)

![C++](https://img.shields.io/badge/C%2B%2B-23-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.31+-green.svg)
![SFML](https://img.shields.io/badge/SFML-3.0.2-orange.svg)
![License](https://img.shields.io/badge/License-MIT-blue.svg)

A modern **CHIP-8 emulator** written in **C++23** with **SFML 3**, featuring a graphical interface, an integrated debugger, and a modular architecture designed for learning emulator development.

---

## Table of Contents

* [Overview](#overview)
* [Features](#features)
* [Architecture](#architecture-1)
* [Memory Layout](#memory-layout)
* [Building](#building)
* [Running](#running)
* [Controls](#controls)
* [Credits](#credits)

---

## Overview

**CHIP-8 Emulator** is an educational project implementing a complete CHIP-8 virtual machine emulator in modern **C++23**.

Originally introduced in the 1970s, CHIP-8 provided a simple platform for developing games on 8-bit computers. Although small, its architecture includes nearly every fundamental concept found in real CPUs:

* registers
* addressable memory
* call stack
* timers
* the **Fetch → Decode → Execute** instruction cycle

The project is intended as a learning resource for emulator development, CPU architecture, and modern C++ design.

---

## Features

* Complete CHIP-8 instruction set implementation
* SFML 3 graphical interface
* Integrated debugger

  * step execution
  * breakpoints
  * disassembler
  * memory viewer
  * register viewer
* Load arbitrary CHIP-8 ROMs
* Unit tests
* Modular architecture

---

## Architecture

The project consists of three independent modules:

```text
core/
ui/
debug/
```

| Module    | Description                                   |
| --------- | --------------------------------------------- |
| **core**  | CPU, memory, timers and instruction execution |
| **ui**    | Rendering and input                           |
| **debug** | Debugger and disassembler                     |

### Design

The `CPU` does **not own** the `Memory` or `FrameBuffer`.

Instead, it stores references to shared objects, allowing the debugger and UI to inspect exactly the same emulator state without duplicating memory.

---

## Memory Layout

|       Address | Purpose       |
| ------------: | ------------- |
| `0x000–0x04F` | Unused        |
| `0x050–0x09F` | Built-in font |
| `0x0A0–0x1FF` | Unused        |
| `0x200–0xFFF` | Program ROM   |

Unlike the original COSMAC VIP, the framebuffer is stored separately from system memory. This prevents instructions such as `FX55` from accidentally overwriting display memory.

---

## Requirements

* C++23 compiler
* CMake 3.31+

Dependencies are automatically downloaded via **FetchContent** if they are not available on the system.

* SFML 3.0.2
* GoogleTest 1.17.0

---

## Building

```bash
git clone https://github.com/Sk9l9tik/CHIP-8_Emulator.git
cd CHIP-8_Emulator

mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

---

## Running

```bash
./chip8_emu <ROM>
```

Examples:

Linux:

```bash
./chip8_emu Tetris_.ch8
```

Windows:

```bash
chip8_emu.exe Tetris_.ch8
```

A ROM can also be loaded from the graphical interface while the emulator is running.

---

## Controls

| CHIP-8    | Keyboard  |
| --------- | --------- |
| `1 2 3 C` | `2 3 4 Z` |
| `4 5 6 D` | `Q W E X` |
| `7 8 9 E` | `R A S C` |
| `A 0 B F` | `D 1 F V` |

---

## Credits

| Avatar                                                       | Contributor                              | Role               |
| ------------------------------------------------------------ | ---------------------------------------- | ------------------ |
| <img src="https://github.com/Sk9l9tik.png" width="50">       | [Sk9l9tik](https://github.com/Sk9l9tik)  | Debugger           |
| <img src="https://github.com/crw884.png" width="50">         | [crw884](https://github.com/crw884)      | UX/UI, SFML   |
| <img src="https://github.com/SosikuKawiiHog.png" width="50"> | [SKH](https://github.com/SosikuKawiiHog) | Core, Architecture |
