#pragma once
#include <stdint.h>

const char* to_string(uint64_t value);
const char* to_string(int64_t value);
const char* para_hstring(uint64_t value);
const char* para_hstring(uint32_t value);
const char* para_hstring(uint16_t value);
const char* para_hstring(uint8_t value);
const char* to_string(double value, uint8_t decimalPlaces);
const char* to_string(double value);
