/**
 * Copyright 2018-2019 OneLoneCoder.com
 */

#include "../include/CPU.hpp"

namespace cloNES {
    CPU::CPU(MedNES::RAM *ram, MedNES::Mapper *mapper, MedNES::PPU *ppu, MedNES::Controller *controller) {
        this->ram = ram;
        this->mapper = mapper;
        this->ppu = ppu;
        this->controller = controller;
    }
    CPU::~CPU() {
        if(ram) delete ram;
        if(mapper) delete mapper;
        if(ppu) delete ppu;
        if(controller) delete controller;
    }

    Byte CPU::getFlag(FLAGS flag) {
        return (regP & flag);
    }

    void CPU::setFlag(FLAGS flag, bool value) {
        if(value) {
            regP |= flag;
        } else {
            regP &= (~flag);
        }
    }

    void CPU::clock() {
        if(cycles == 0) {
            opcode = read(regPC);
            regPC++;

            cycles = SHEET[opcode].CYCLE;

            Byte addCyc1 = (this->*SHEET[opcode].OPCODE)();
            Byte addCyc2 = (this->*SHEET[opcode].ADMODE)();
            cycles += (addCyc1 & addCyc2);
        }
        cycles--;
    }

    void CPU::reset() {
        addrAbs = 0xFFFC;
        Byte lo = read(addrAbs);
        Byte hi = read(addrAbs + 1);
        regPC = (hi << 8) | lo;

        // Reset internal registers
        regA = 0;
        regX = 0;
        regY = 0;
        regS = 0xFD;
        regP = 0x00 | U;

        // Clear internal helper variables
        addrRel = 0x0000;
        addrAbs = 0x0000;

        // Reset takes time
        cycles = 8;
    }
    /**
     * nterrupt requests are a complex operation and only happen if the
     * "disable interrupt" flag is 0. 
     */
    void CPU::IRQ() {
        if(getFlag(I) == 0) { 
            write(0x0100 + regS, (regPC >> 8) & 0x00FF);
            regS--;
            write(0x0100 + regS, regPC & 0x00FF);
            regS--;

            setFlag(B, 0);
            setFlag(U, 1);
            setFlag(I, 1);
            write(0x0100 + regS, regP);
            regS--;

            addrAbs = 0xFFFE;
            Byte lo = read(addrAbs);
            Byte hi = read(addrAbs + 1);
            regPC = (hi << 8) | lo;

            cycles = 7;
        }
    }
    /**
     * A Non-Maskable Interrupt cannot be ignored. 
     */
    void CPU::NMI() {
        write(0x0100 + regS, (regPC >> 8) & 0x00FF);
        regS--;
        write(0x0100 + regS, regPC & 0x00FF);
        regS--;

        setFlag(B, 0);
        setFlag(U, 1);
        setFlag(I, 1);
        write(0x0100 + regS, regP);
        regS--;

        addrAbs = 0xFFFA;
        Byte lo = read(addrAbs);
        Byte hi = read(addrAbs + 1);
        regPC = (hi << 8) | lo;

        cycles = 8;
    }

    ////////////////////////////////////////////////////
    // Addressing mode. 
    ////////////////////////////////////////////////////

    Byte CPU::IMP() {
        return 0;
    }
    Byte CPU::IMM() {
        addrAbs = regPC++;
        return 0;
    }
    Byte CPU::ZP0() {
        addrAbs = read(regPC++);
        addrAbs &= 0x00FF;
        return 0;
    }
    Byte CPU::ZPX() {
        addrAbs = (read(regPC++) + regX);
        addrAbs &= 0x00FF;
        return 0;
    }
    Byte CPU::ZPY() {
        addrAbs = (read(regPC++) + regY);
        addrAbs &= 0x00FF;
        return 0;
    }
    Byte CPU::REL() {
        addrRel = read(regPC++);
        if(addrRel & 0x80) {
            addrRel |= 0xFF00;
        }
        return 0;
    }
    Byte CPU::ABS() {
        Word lo = read(regPC++);
        Word hi = read(regPC++);
        addrAbs = (hi << 8) | lo;
        return 0;
    }
    Byte CPU::ABX() {
        Word lo = read(regPC++);
        Word hi = read(regPC++);
        addrAbs = (hi << 8) | lo;
        addrAbs += regX;
        if((addrAbs & 0xFF00) != (hi << 8)) {
            return 1;
        }
        return 0;
    }
    Byte CPU::ABY() {
        Word lo = read(regPC++);
        Word hi = read(regPC++);
        addrAbs = (hi << 8) | lo;
        addrAbs += regY;
        if((addrAbs & 0xFF00) != (hi << 8)) {
            return 1;
        }
        return 0;
    }
    Byte CPU::IND() {
        Word ptrLo = read(regPC++);
        Word ptrHi = read(regPC++);
        Word ptr = (ptrHi << 8) | ptrLo;
        addrAbs = (read(ptr + 1) << 8) | read(ptr);
        return 0;
    }
    Byte CPU::IZX() {
        Word t = read(regPC++);
        Word lo = read((t + (Word)regX) & 0x00FF);
        Word hi = read((t + (Word)regX + 1) & 0x00FF);
        addrAbs = (hi << 8) | lo;
        return 0;
    }
    Byte CPU::IZY() {
        Word t = read(regPC++);
        Word lo = read(t & 0x00FF);
        Word hi = read((t + 1) & 0x00FF);
        addrAbs = (hi << 8) | lo;
        addrAbs += regY;
        if((addrAbs & 0xFF00) != (hi << 8)) {
            return 1;
        }
        return 0;
    }

    Byte CPU::memoryAccess(AccessMode mode, Word address, Byte data) {
        Byte readData = 0;

        /**
         * Address Range: $0000-$07FF
         * Size: $0800
         * Device: 2KB internal RAM. 
         */
        if(address >= 0x0000 && address < 0x2000) {
            address %= 0x0800;
            if(mode == AccessMode::READ) {
                readData = ram->read(address);
            } else {
                ram->write(address, data);
            }
        }

        /**
         * Address Range: $2000-$2007
         * Size: $0008
         * Device: NES PPU registers. 
         */
        else if(address >= 0x2000 && address < 0x4000) {
            address %= 0x0008; 
            if(mode == AccessMode::READ) {
                readData = ppu->read(address);
            } else {
                ppu->write(address, data);
            }
        }

        /**
         * Address Range: $4000-$4017
         * Size: $0018
         * Device: NES APU and I/O registers. 
         */
        else if(address >= 0x4000 && address < 0x4018) {
            //COPY OAM
            if(address == 0x4014) {
                if(mode == AccessMode::WRITE) {
                    ppu->write(address, data);
                    for (int i = 0; i < 256; i++) {
                        ppu->copyOAM(read(data * 256 + i), i);
                    }
                }
            } else {
                if(mode == AccessMode::READ) {
                    readData = controller->read(address);
                } else {
                    controller->write(address, data);
                }
            }
            //APU I/O registers
        } 

        /**
         * Address Range: $4018-$401F
         * Size: $0008
         * Device: APU and I/O functionality that is normally disabled.
         * (CPU test mode)
         */
        else if(address >= 0x4018 && address < 0x4020) {} 

        /**
         * Address Range: $6000–$7FFF
         * Size: $2000
         * Device: Usually cartridge RAM, when present. 
         * 
         * Address Range: $8000–$FFFF
         * Size: $8000
         * Device: Usually cartridge ROM and mapper registers.
         */
        else if(address >= 0x6000 && address <= 0xFFFF) {
            if(mode == AccessMode::READ) {
                readData = mapper->read(address);
            } else {
                mapper->write(address, data);
            }
        }

        return readData;
    }

    Byte CPU::read(Word address) {
        Byte r = memoryAccess(AccessMode::READ, address, 0);
        return r;
    }
    void CPU::write(Word address, Byte data) {
        memoryAccess(AccessMode::WRITE, address, data);
    }

    ////////////////////////////////////////////////////
    // Instructions. 
    ////////////////////////////////////////////////////

    Byte CPU::fetch() {
        if(SHEET[opcode].ADMODE != &CPU::IMP) {
            return read(addrAbs);
        }
        return regA;
    }

    /**
     * Add with Carry In. 
     */
    Byte CPU::ADC() {
        Byte memory = fetch();
        Word A = regA + memory + getFlag(C);
        setFlag(C, A > 0xFF);
        setFlag(Z, (A & 0x00FF) == 0);
        setFlag(V, (~(regA ^ memory) & (regA ^ A)) & 0x0080);
        setFlag(N, A & 0x80);
        regA = A & 0x00FF;
        return 1;
    }
    /**
     * Bitwise Logic AND. 
     */
    Byte CPU::AND() {
        Byte memory = fetch();
        regA &= memory;
        setFlag(Z, regA == 0x00);
        setFlag(N, regA & 0x80);
        return 1;
    }
    /**
     * Arithmetic Shift Left. 
     */
    Byte CPU::ASL() {
        Byte memory = fetch();
        Word A = memory << 1;
        setFlag(C, (A & 0xFF00) > 0);
        setFlag(Z, (A & 0x00FF) == 0x00);
        setFlag(N, A & 0x80);
        if(SHEET[opcode].ADMODE == &CPU::IMP) {
            regA = A & 0x00FF;
        } else {
            write(addrAbs, A & 0x00FF);
        }
        return 0;
    }
    /**
     * Branch if Carry Clear. 
     */
    Byte CPU::BCC() {
        if(getFlag(C) == 0) {
            cycles++;
            addrAbs = regPC + addrRel;
            if((addrAbs & 0xFF00) != (regPC & 0xFF00)) {
                cycles++;
            }
            regPC = addrAbs;
        }
        return 0;
    }
    /**
     * Branch if Carry Set. 
     */
    Byte CPU::BCS() {
        if(getFlag(C) == 1) {
            cycles++;
            addrAbs = regPC + addrRel;
            if ((addrAbs & 0xFF00) != (regPC & 0xFF00)) {
                cycles++;
            }
            regPC = addrAbs;
        }
        return 0;
    }
    /**
     * Branch if Equal. 
     */
    Byte CPU::BEQ() {
        if(getFlag(Z) == 1) {
            cycles++;
            addrAbs = regPC + addrRel;
            if ((addrAbs & 0xFF00) != (regPC & 0xFF00)) {
                cycles++;
            }
            regPC = addrAbs;
        }
        return 0;
    }
    /**
     * Bit Test. 
     */
    Byte CPU::BIT() {
        Byte memory = fetch();
        Byte temp = regA & memory;
        setFlag(Z, (temp & 0x00FF) == 0x00);
        setFlag(N, memory & N);
        setFlag(V, memory & V);
        return 0;
    }
    /**
     * Branch if Negative. 
     */
    Byte CPU::BMI() {
        if(getFlag(N) == 1) {
            cycles++;
            addrAbs = regPC + addrRel;
            if ((addrAbs & 0xFF00) != (regPC & 0xFF00)) {
                cycles++;
            }
            regPC = addrAbs;
        }
        return 0;
    }
    /**
     * Branch if Not Equal. 
     */
    Byte CPU::BNE() {
        if(getFlag(Z) == 0) {
            cycles++;
            addrAbs = regPC + addrRel;
            if ((addrAbs & 0xFF00) != (regPC & 0xFF00)) {
                cycles++;
            }
            regPC = addrAbs;
        }
        return 0;
    }
    /**
     * Branch if Positive. 
     */
    Byte CPU::BPL() {
        if(getFlag(N) == 0) {
            cycles++;
            addrAbs = regPC + addrRel;
            if ((addrAbs & 0xFF00) != (regPC & 0xFF00)) {
                cycles++;
            }
            regPC = addrAbs;
        }
        return 0;
    }
    /**
     * Break. 
     */
    Byte CPU::BRK() {
        regPC++;
	
        setFlag(I, 1);
        write(0x0100 + regS, (regPC >> 8) & 0x00FF);
        regS--;
        write(0x0100 + regS, regPC & 0x00FF);
        regS--;

        setFlag(B, 1);
        write(0x0100 + regS, regP);
        regS--;
        setFlag(B, 0);

        regPC = read(0xFFFE) | (read(0xFFFF) << 8);
        return 0;
    }
    /**
     * Branch if Overflow Clear. 
     */
    Byte CPU::BVC() {
        if(getFlag(V) == 0) {
            cycles++;
            addrAbs = regPC + addrRel;
            if((addrAbs & 0xFF00) != (regPC & 0xFF00)) {
                cycles++;
            }
            regPC = addrAbs;
        }
        return 0;
    }
    /**
     * Branch if Overflow Set. 
     */
    Byte CPU::BVS() {
        if(getFlag(V) == 1) {
            cycles++;
            addrAbs = regPC + addrRel;
            if((addrAbs & 0xFF00) != (regPC & 0xFF00)) {
                cycles++;
            }
            regPC = addrAbs;
        }
        return 0;
    }
    /**
     * Clear Carry Flag. 
     */
    Byte CPU::CLC() {
        setFlag(C, false);
	    return 0;
    }
    /**
     * Clear Decimal Flag. 
     */
    Byte CPU::CLD() {
        setFlag(D, false);
	    return 0;
    }
    /**
     * Disable Interrupts / Clear Interrupt Flag. 
     */
    Byte CPU::CLI() {
        setFlag(I, false);
	    return 0;
    }
    /**
     * Clear Overflow Flag. 
     */
    Byte CPU::CLV() {
        setFlag(V, false);
	    return 0;
    }
    /**
     * Compare Accumulator. 
     */
    Byte CPU::CMP() {
        Byte memory = fetch();
        Word temp = regA - memory;
        setFlag(C, regA >= memory);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        return 1;
    }
    /**
     * Compare X Register. 
     */
    Byte CPU::CPX() {
        Byte memory = fetch();
        Word temp = regX - memory;
        setFlag(C, regX >= memory);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        return 0;
    }
    /**
     * Compare Y Register. 
     */
    Byte CPU::CPY() {
        Byte memory = fetch();
        Word temp = regY - memory;
        setFlag(C, regY >= memory);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        return 0;
    }
    /**
     * Decrement Value at Memory Location. 
     */
    Byte CPU::DEC() {
        Byte memory = fetch();
        Word temp = memory - 1;
        write(addrAbs, temp & 0x00FF);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        return 0;
    }
    /**
     * Decrement X Register. 
     */
    Byte CPU::DEX() {
        regX--;
        setFlag(Z, regX == 0x00);
        setFlag(N, regX & 0x80);
        return 0;
    }
    /**
     * Decrement Y Register. 
     */
    Byte CPU::DEY() {
        regY--;
        setFlag(Z, regY == 0x00);
        setFlag(N, regY & 0x80);
        return 0;
    }
    /**
     * Bitwise Logic XOR. 
     */
    Byte CPU::EOR() {
        Byte memory = fetch();
        regA = regA ^ memory;	
        setFlag(Z, regA == 0x00);
        setFlag(N, regA & 0x80);
        return 1;
    }
    /**
     * Increment Value at Memory Location. 
     */
    Byte CPU::INC() {
        Byte memory = fetch();
        Word temp = memory + 1;
        write(addrAbs, temp & 0x00FF);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        return 0;
    }
    /**
     * Increment X Register. 
     */
    Byte CPU::INX() {
        regX++;
        setFlag(Z, regX == 0x00);
        setFlag(N, regX & 0x80);
        return 0;
    }
    /**
     * Increment Y Register. 
     */
    Byte CPU::INY() {
        regY++;
        setFlag(Z, regY == 0x00);
        setFlag(N, regY & 0x80);
        return 0;
    }
    /**
     * Jump To Location. 
     */
    Byte CPU::JMP() {
        regPC = addrAbs;
	    return 0;
    }
    /**
     * Jump To Sub-Routine. 
     */
    Byte CPU::JSR() {
        regPC--;

        write(0x0100 + regS, (regPC >> 8) & 0x00FF);
        regS--;
        write(0x0100 + regS, regPC & 0x00FF);
        regS--;

        regPC = addrAbs;
        return 0;
    }
    /**
     * Load The Accumulator. 
     */
    Byte CPU::LDA() {
        Byte memory = fetch();
        regA = memory;
        setFlag(Z, regA == 0x00);
        setFlag(N, regA & 0x80);
        return 1;
    }
    /**
     * Load The X Register. 
     */
    Byte CPU::LDX() {
        Byte memory = fetch();
        regX = memory;
        setFlag(Z, regX == 0x00);
        setFlag(N, regX & 0x80);
        return 1;
    }
    /**
     * Load The Y Register. 
     */
    Byte CPU::LDY() {
        Byte memory = fetch();
        regY = memory;
        setFlag(Z, regY == 0x00);
        setFlag(N, regY & 0x80);
        return 1;
    }
    /**
     * Logical Shift Right. 
     */
    Byte CPU::LSR() {
        Byte memory = fetch();
        setFlag(C, memory & 0x0001);
        Word temp = memory >> 1;	
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        if(SHEET[opcode].ADMODE == &CPU::IMP) {
            regA = temp & 0x00FF;
        } else {
            write(addrAbs, temp & 0x00FF);
        }
        return 0;
    }
    /**
     * No Operation. 
     */
    Byte CPU::NOP() {
        switch(opcode) {
            case 0x1C:
            case 0x3C:
            case 0x5C:
            case 0x7C:
            case 0xDC:
            case 0xFC: return 1;
        }
        return 0;
    }
    /**
     * Bitwise Logic OR. 
     */
    Byte CPU::ORA() {
        Byte memory = fetch();
        regA = regA | memory;
        setFlag(Z, regA == 0x00);
        setFlag(N, regA & 0x80);
        return 1;
    }
    /**
     * Push Accumulator to Stack. 
     */
    Byte CPU::PHA() {
        write(0x0100 + regS, regA);
        regS--;
        return 0;
    }
    /**
     * Push Status Register to Stack. 
     */
    Byte CPU::PHP() {
        write(0x0100 + regS, regP | B | U);
        setFlag(B, 0);
        setFlag(U, 0);
        regS--;
        return 0;
    }
    /**
     * Pop Accumulator off Stack. 
     */
    Byte CPU::PLA() {
        regS++;
        regA = read(0x0100 + regS);
        setFlag(Z, regA == 0x00);
        setFlag(N, regA & 0x80);
        return 0;
    }
    /**
     * Pop Status Register off Stack. 
     */
    Byte CPU::PLP() {
        regS++;
        regP = read(0x0100 + regS);
        setFlag(U, 1);
        return 0;
    }
    /**
     * Rotate Left. 
     */
    Byte CPU::ROL() {
        Byte memory = fetch();
        Word temp = (memory << 1) | getFlag(C);
        setFlag(C, temp & 0xFF00);
        setFlag(Z, (temp & 0x00FF) == 0x0000);
        setFlag(N, temp & 0x0080);
        if (SHEET[opcode].ADMODE == &CPU::IMP) {
            regA = temp & 0x00FF;
        } else {
            write(addrAbs, temp & 0x00FF);
        }
        return 0;
    }
    /**
     * Rotate Right. 
     */
    Byte CPU::ROR() {
        Byte memory = fetch();
        Word temp = (getFlag(C) << 7) | (memory >> 1);
        setFlag(C, memory & 0x01);
        setFlag(Z, (temp & 0x00FF) == 0x00);
        setFlag(N, temp & 0x0080);
        if (SHEET[opcode].ADMODE == &CPU::IMP) {
            regA = temp & 0x00FF;
        } else {
            write(addrAbs, temp & 0x00FF);
        }
        return 0;
    }
    /**
     * Return from Interrupt. 
     */
    Byte CPU::RTI() {
        regS++;
        regP = read(0x0100 + regS);
        regP &= ~B;
        regP &= ~U;

        regS++;
        regPC = read(0x0100 + regS);
        regS++;
        regPC |= read(0x0100 + regS) << 8;
        return 0;
    }
    /**
     * Return from Subroutine. 
     */
    Byte CPU::RTS() {
        regS++;
        regPC = read(0x0100 + regS);
        regS++;
        regPC |= read(0x0100 + regS) << 8;
        
        regPC++;
        return 0;
    }
    /**
     * Subtraction with Borrow In. 
     */
    Byte CPU::SBC() {
        Byte memory = fetch();
        Word value = memory ^ 0x00FF;
        Word A = regA + value + getFlag(C);
        setFlag(C, A & 0xFF00);
        setFlag(Z, ((A & 0x00FF) == 0));
        setFlag(V, (A ^ regA) & (A ^ value) & 0x0080);
        setFlag(N, A & 0x0080);
        regA = A & 0x00FF;
        return 1;
    } 
    /**
     * Set Carry Flag. 
     */
    Byte CPU::SEC() {
        setFlag(C, true);
	    return 0;
    }
    /**
     * Set Decimal Flag. 
     */
    Byte CPU::SED() {
        setFlag(D, true);
        return 0;
    }
    /**
     * Set Interrupt Flag / Enable Interrupts. 
     */
    Byte CPU::SEI() {
        setFlag(I, true);
	    return 0;
    }
    /**
     * Store Accumulator at Address. 
     */
    Byte CPU::STA() {
        write(addrAbs, regA);
        return 0;
    }
    /**
     * Store X Register at Address. 
     */
    Byte CPU::STX() {
        write(addrAbs, regX);
        return 0;
    }
    /**
     * Store Y Register at Address. 
     */
    Byte CPU::STY() {
        write(addrAbs, regY);
        return 0;
    }
    /**
     * Transfer Accumulator to X Register. 
     */
    Byte CPU::TAX() {
        regX = regA;
        setFlag(Z, regX == 0x00);
        setFlag(N, regX & 0x80);
        return 0;
    }
    /**
     * Transfer Accumulator to Y Register. 
     */
    Byte CPU::TAY() {
        regY = regA;
        setFlag(Z, regY == 0x00);
        setFlag(N, regY & 0x80);
        return 0;
    }
    /**
     * Transfer Stack Pointer to X Register. 
     */
    Byte CPU::TSX() {
        regX = regS;
        setFlag(Z, regX == 0x00);
        setFlag(N, regX & 0x80);
        return 0;
    }
    /**
     * Transfer X Register to Accumulator. 
     */
    Byte CPU::TXA() {
        regA = regX;
        setFlag(Z, regA == 0x00);
        setFlag(N, regA & 0x80);
        return 0;
    }
    /**
     * Transfer X Register to Stack Pointer. 
     */
    Byte CPU::TXS() {
        regS = regX;
	    return 0;
    }
    /**
     * Transfer Y Register to Accumulator. 
     */
    Byte CPU::TYA() {
        regA = regY;
        setFlag(Z, regA == 0x00);
        setFlag(N, regA & 0x80);
        return 0;
    }
    /**
     * This function captures illegal opcodes. 
     */
    Byte CPU::XXX() {
        return 0;
    }
    
} // namespace cloNES 
