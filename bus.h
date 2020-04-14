#pragma once
#include<cstdint>
#include<array>

class bus
{

public:
    bus();
    ~bus();

    6502proc cpu;
    std:: array<uint8_t,64*1024> ram;

    void write(uint16_t addr,uint8_t data);
    uint8_t read(uint16_t addr);

};

bus::bus()
{
    cpu.loadBus(this);
    for(auto &i : ram)=0x00;
}

bus::~bus()
{

}

void bus::write(uint16_t addr,uint8_t data)
{
    if(addr>=0x0000 && addr<= 0xFFFF)
        ram[addr]=data;
}

uint8_t read(uint16_t addr)
{
    if(addr>=0x0000 && addr<=0xFFFF)
        return ram[addr];
    return 0x00;
}




