
#include "memory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntradas, uint64_t mMapTamanhoDesc){

    static uint64_t tamanhoMemoriaEmBytes = 0;
    if (tamanhoMemoriaEmBytes > 0) return tamanhoMemoriaEmBytes;

    for (int i = 0; i < mMapEntradas; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapTamanhoDesc));
        tamanhoMemoriaEmBytes += desc->numeroPaginas * 4096;
    }

    return tamanhoMemoriaEmBytes;

}

void memset(void* start, uint8_t value, uint64_t num){
    for (uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}