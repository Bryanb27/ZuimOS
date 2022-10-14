#include "paging.h"

void EntradaDiretorioDePagina::SetFlag(PT_Flag flag, bool enabled){
    uint64_t seletorDeBit = (uint64_t)1 << flag;
    Value &= ~seletorDeBit;
    if (enabled){
        Value |= seletorDeBit;
    }
}

bool EntradaDiretorioDePagina::GetFlag(PT_Flag flag){
    uint64_t seletorDeBit = (uint64_t)1 << flag;
    return Value & seletorDeBit > 0 ? true : false;
}

uint64_t EntradaDiretorioDePagina::GetEndereco(){
    return (Value & 0x000ffffffffff000) >> 12;
}

void EntradaDiretorioDePagina::SetAddress(uint64_t address){
    address &= 0x000000ffffffffff;
    Value &= 0xfff0000000000fff;
    Value |= (address << 12);
}