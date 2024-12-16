# cloNES

**cloNES** is an NES emulator fork from [MedNES](https://github.com/wpmed92/MedNES), 
which is a cycle-accurate NES emulator written in C++. 

Supported mappers: 
- NROM(0), 
- MMC1(1), 
- UNROM(2), 
- CNROM(3). 

## Getting Started ##

These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites ###

* [libSDL2](https://www.libsdl.org/download-2.0.php)

### Build ### 

```bash
$ git clone https://github.com/ohnun/cloNES.git
$ cd cloNES
$ make -j8 # or make
```

### Execute ###

```bash
# In the cloNES folder. 
$ ./cloNES <path/to/rom>
```

### Usage

| keyboard     | function |
| :-:          | :-:      |
| <kbd>W</kbd> | UP       |
| <kbd>S</kbd> | DOWN     |
| <kbd>A</kbd> | LEFT     |
| <kbd>D</kbd> | RIGHT    |
| <kbd>E</kbd> | Start    |
| <kbd>Q</kbd> | Select   |
| <kbd>J</kbd> | A        |
| <kbd>K</kbd> | B        |
| <kbd>X</kbd> | Quit     |

## References

1. [Re: 从零开始的红白机模拟 - [02]ROM](https://github.com/dustpg/BlogFM/issues/6)
1. [6502 Instruction Set](https://www.masswerk.at/6502/6502_instruction_set.html) 
1. [NES基本原理 (一) 总述](https://www.nowcoder.com/discuss/565531965722472448)
1. [NES基本原理 (二) CPU](https://www.nowcoder.com/discuss/565551215161942016)
1. [NES基本原理 (三) PPU](https://www.nowcoder.com/discuss/565553764464091136)
1. [NES基本原理 (六) 手柄](https://www.nowcoder.com/discuss/565558934224777216)
1. [NES基本原理 (七) Mapper](https://www.nowcoder.com/discuss/565560219778940928)
1. [NES基本原理 (八) MUSIC](https://www.nowcoder.com/discuss/565561509040906240)

## License

Copyright &copy; 2024 Ohnun

This project is [MIT](./LICENSE)

