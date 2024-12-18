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
$ make -j12 # or make
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

1. [NES programming guide](https://www.nesdev.org/wiki/Programming_guide)
1. [6502 Instruction Set](https://www.masswerk.at/6502/6502_instruction_set.html)

## License

Copyright &copy; 2024 Ohnun

This project is [MIT](./LICENSE)

