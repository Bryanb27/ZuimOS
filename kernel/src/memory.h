#pragma once

#include <stdint.h>
#include "efiMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntradas, uint64_t mMapTamanhoDesc);
void memset(void* start, uint8_t value, uint64_t num);