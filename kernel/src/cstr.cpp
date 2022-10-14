#include "cstr.h"

char uintPara_StringSaida[128];
const char* to_string(uint64_t value){
    uint8_t size;
    uint64_t testeTamanho = value;
    while (testeTamanho / 10 > 0){
        testeTamanho /= 10;
        size++;
    }

    uint8_t index = 0;
    while(value / 10 > 0){
        uint8_t lembrete = value % 10;
        value /= 10;
        uintPara_StringSaida[size - index] = lembrete + '0';
        index++;
    }
    uint8_t lembrete = value % 10;
    uintPara_StringSaida[size - index] = lembrete + '0';
    uintPara_StringSaida[size + 1] = 0; 
    return uintPara_StringSaida;
}

char hexPara_StringSaida[128];
const char* para_hstring(uint64_t value){
    uint64_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexPara_StringSaida[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexPara_StringSaida[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexPara_StringSaida[size + 1] = 0;
    return hexPara_StringSaida;
}

char hexPara_StringSaida32[128];
const char* para_hstring(uint32_t value){
    uint32_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexPara_StringSaida32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexPara_StringSaida32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexPara_StringSaida32[size + 1] = 0;
    return hexPara_StringSaida32;
}

char hexPara_StringSaida16[128];
const char* para_hstring(uint16_t value){
    uint16_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexPara_StringSaida16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexPara_StringSaida16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexPara_StringSaida16[size + 1] = 0;
    return hexPara_StringSaida16;
}

char hexPara_StringSaida8[128];
const char* para_hstring(uint8_t value){
    uint8_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexPara_StringSaida8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexPara_StringSaida8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexPara_StringSaida8[size + 1] = 0;
    return hexPara_StringSaida8;
}

char intPara_StringSaida[128];
const char* to_string(int64_t value){
    uint8_t isNegative = 0;

    if (value < 0){
        isNegative = 1;
        value *= -1;
        intPara_StringSaida[0] = '-';
    }

    uint8_t size;
    uint64_t testeTamanho = value;
    while (testeTamanho / 10 > 0){
        testeTamanho /= 10;
        size++;
    }

    uint8_t index = 0;
    while(value / 10 > 0){
        uint8_t lembrete = value % 10;
        value /= 10;
        intPara_StringSaida[isNegative + size - index] = lembrete + '0';
        index++;
    }
    uint8_t lembrete = value % 10;
    intPara_StringSaida[isNegative + size - index] = lembrete + '0';
    intPara_StringSaida[isNegative + size + 1] = 0; 
    return intPara_StringSaida;
}

char doubleTo_StringOutput[128];
const char* to_string(double value, uint8_t decimalPlaces){
    if (decimalPlaces > 20) decimalPlaces = 20;

    char* intPtr = (char*)to_string((int64_t)value);
    char* doublePtr = doubleTo_StringOutput;

    if (value < 0){
        value *= -1;
    }

    while(*intPtr != 0){
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }

    *doublePtr = '.';
    doublePtr++;

    double newValue = value - (int)value;

    for (uint8_t i = 0; i < decimalPlaces; i++){
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = 0;
    return doubleTo_StringOutput;
}

const char* to_string(double value){
    return to_string(value, 2);
}
