#pragma once

#include <stdint.h>
#include <stdio.h>

#include <bitset>
#include <string>
#include <vector>

#include "INESBus.hpp"

namespace MedNES {

class Mapper;

struct INESHeader {
    //Header 16 byte
    char nes[4];
    u8 prgIn16kb;
    u8 chrIn8kb;
    u8 flags6;
    u8 flags7;
    u8 prgRamIn8kb;
    u8 flags9;
    u8 flags10;
    char zeros[5];
};

class ROM {
public:
    ROM(const std::string filePath);
    ROM(const std::vector<u8> &byteStream);

    std::vector<u8> getChrData() { return chrData; };
    std::vector<u8> getPrgCode() { return prgCode; };
    void printHeader();
    int getMirroring();
    Mapper *getMapper();

private:
    void open(const std::string filePath);
    void openWithByte(const std::vector<u8> &byteStream);

private:
    INESHeader header;
    std::vector<u8> trainer;
    std::vector<u8> prgCode;
    std::vector<u8> chrData;
    int mirroring;
    u8 mapperNum;
};

};  //namespace MedNES
