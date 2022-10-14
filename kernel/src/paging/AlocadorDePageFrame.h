#pragma once
#include "../efiMemory.h"
#include <stdint.h>
#include "../Bitmap.h"
#include "../memory.h"

class AlocadorDePageFrame {
    public:
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapTamanho, size_t mMapTamanhoDesc);
    Bitmap PageBitmap;
    void FreePage(void* address);
    void FreePages(void* address, uint64_t pageCount);
    void LockPage(void* address);
    void LockPages(void* address, uint64_t pageCount);
    void* RequisitarPagina();
    uint64_t GetFreeRAM();
    uint64_t GetUsedRAM();
    uint64_t GetReservedRAM();


    private:
    void InitBitmap(size_t bitmapSize, void* bufferAddress);
    void ReservePage(void* address);
    void ReservePages(void* address, uint64_t pageCount);
    void UnreservePage(void* address);
    void UnreservePages(void* address, uint64_t pageCount);

};

extern AlocadorDePageFrame AlocadorGlobal;