# cloNES

cloNES is a fork from MedNES. 
MedNES is a cycle-accurate NES emulator written in C++.
Supported mappers: NROM(0), MMC1(1), UNROM(2), CNROM(3)

## Getting Started ##
These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites ###
* **[GIT](https://git-scm.com)**
* **[libSDL2](https://www.libsdl.org/download-2.0.php)**

### Cloning This Repository ###
1. Open Terminal.
2. Change the current working directory to the location where you want the cloned directory to be made.
3. Type `git clone https://github.com/ohnun/cloNES.git`
4. Press Enter. *Your local clone will be created*.

**Build**

`make` or `make -j8` 

**Execute**

`./cloNES -insert <path/to/rom>`

### Usage

- arrow keys, 
- enter (start), 
- space (select), 
- X (A), 
- C (B). 

## License
>This program is free software: you can redistribute it and/or modify it under the terms of the MIT License.

>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [MIT License](https://en.wikipedia.org/wiki/MIT_License) for more details.

