#include "../include/ROM.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../include/Mapper/CNROM.hpp"
#include "../include/Mapper/MMC1.hpp"
#include "../include/Mapper/NROM.hpp"
#include "../include/Mapper/UnROM.hpp"

namespace MedNES {

void ROM::open(std::string filePath) {

    // Read the file <filePath> as binary into the <in> object. 
    std::ifstream in(filePath, std::ios::binary);

    //Read header (16 bytes)
    in.read(reinterpret_cast<char *>(&header.nes), sizeof(char[4]));
    in.read(reinterpret_cast<char *>(&header.prgIn16kb), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.chrIn8kb), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.flags6), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.flags7), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.prgRamIn8kb), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.flags9), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.flags10), sizeof(u8));
    in.read(reinterpret_cast<char *>(&header.zeros), sizeof(char[5]));

    trainer.reserve(512);

    int prgSize = header.prgIn16kb * 16384;
    prgCode.resize(prgSize);

    int chrSize = header.chrIn8kb * 8192;
    chrData.resize(chrSize);

    mirroring = header.flags6 & 1;

    //If trainer present
    if ((header.flags6 >> 2) & 1) {
        in.read((char *)trainer.data(), 512);
    }

    in.read((char *)prgCode.data(), header.prgIn16kb * 16384);

    if (header.chrIn8kb > 0) {
        in.read((char *)chrData.data(), header.chrIn8kb * 8192);
    } else {
        chrData = std::vector<u8>(8192, 0);
    }
}

void ROM::openWithByte(const std::vector<u8>& byteStream) {
    // 创建一个内存流来读取字节流
    std::istringstream inStream(std::string(byteStream.begin(), byteStream.end()), std::ios::binary);

    inStream.read(reinterpret_cast<char *>(&header.nes), sizeof(char[4]));
    inStream.read(reinterpret_cast<char *>(&header.prgIn16kb), sizeof(u8));
    inStream.read(reinterpret_cast<char *>(&header.chrIn8kb), sizeof(u8));
    inStream.read(reinterpret_cast<char *>(&header.flags6), sizeof(u8));
    inStream.read(reinterpret_cast<char *>(&header.flags7), sizeof(u8));
    inStream.read(reinterpret_cast<char *>(&header.prgRamIn8kb), sizeof(u8));
    inStream.read(reinterpret_cast<char *>(&header.flags9), sizeof(u8));
    inStream.read(reinterpret_cast<char *>(&header.flags10), sizeof(u8));
    inStream.read(reinterpret_cast<char *>(&header.zeros), sizeof(char[5]));

    trainer.reserve(512);

    int prgSize = header.prgIn16kb * 16384;
    prgCode.resize(prgSize);

    int chrSize = header.chrIn8kb * 8192;
    chrData.resize(chrSize);

    mirroring = header.flags6 & 1;

    if ((header.flags6 >> 2) & 1) {
        inStream.read((char *)trainer.data(), 512);
    }

    inStream.read((char *)prgCode.data(), header.prgIn16kb * 16384);

    if (header.chrIn8kb > 0) {
        inStream.read((char *)chrData.data(), header.chrIn8kb * 8192);
    } else {
        chrData = std::vector<u8>(8192, 0);
    }
}


void ROM::printHeader() {
    std::cout << "<<Header>>"
              << "\n";
    std::cout << "Signature: " << header.nes << "\n";
    std::cout << "PRG ROM (program code) size: " << (int)header.prgIn16kb << " x 16kb \n";
    std::cout << "CHR ROM (graphical data) size: " << (int)header.chrIn8kb << " x 8kb \n";
    mapperNum = ((header.flags6 & 0xF0) >> 4) | (header.flags7 & 0xF0);
    std::bitset<8> flags6Bits(header.flags6);
    std::bitset<8> flags7Bits(header.flags7);
    std::cout << "Flags 6: " << flags6Bits << "\n";
    std::cout << "Flags 7: " << flags7Bits << "\n";
}

int ROM::getMirroring() {
    return mirroring;
}

Mapper *ROM::getMapper() {
    switch (mapperNum) {
        case 0:
            return new NROM(prgCode, chrData, mirroring);
            break;

        case 1:
            return new MMC1(prgCode, chrData, mirroring);
            break;

        case 2:
            return new UnROM(prgCode, chrData, mirroring);
            break;

        case 3:
            return new CNROM(prgCode, chrData, mirroring);
            break;

        default:
            //Unsupported mapper:
            return NULL;
            break;
    }
}

}  //namespace MedNES
