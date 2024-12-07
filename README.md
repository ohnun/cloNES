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

### Build And Execute ### 

```bash
# Build the project. 
$ git clone https://github.com/ohnun/cloNES.git`
$ cd cloNES
$ make -j8 # or make

# Execute in cloNES/ folder. 
$ ./cloNES -insert <path/to/rom>
```

### Usage

- arrow keys, 
- enter (start), 
- space (select), 
- X (A), 
- C (B). 

## License
>This program is free software: you can redistribute it and/or modify it under the terms of the MIT License.
