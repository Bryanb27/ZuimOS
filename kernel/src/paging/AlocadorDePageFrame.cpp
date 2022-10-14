#include "AlocadorDePageFrame.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
bool Initialized = false;
AlocadorDePageFrame AlocadorGlobal;

void AlocadorDePageFrame::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapTamanho, size_t mMapTamanhoDesc){
    if (Initialized) return;

    Initialized = true;

    uint64_t mMapEntradas = mMapTamanho / mMapTamanhoDesc;

    void* largestFreeMemSeg = NULL;
    size_t largestFreeMemSegSize = 0;

    for (int i = 0; i < mMapEntradas; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapTamanhoDesc));
        if (desc->type == 7){ // type = EfiConventionalMemory
            if (desc->numeroPaginas * 4096 > largestFreeMemSegSize)
            {
                largestFreeMemSeg = desc->physAddr;
                largestFreeMemSegSize = desc->numeroPaginas * 4096;
            }
        }
    }

    uint64_t memorySize = GetMemorySize(mMap, mMapEntradas, mMapTamanhoDesc);
    freeMemory = memorySize;
    uint64_t bitmapSize = memorySize / 4096 / 8 + 1;

    InitBitmap(bitmapSize, largestFreeMemSeg);

    LockPages(&PageBitmap, PageBitmap.Size / 4096 + 1);

    for (int i = 0; i < mMapEntradas; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapTamanhoDesc));
        if (desc->type != 7){ // not efiConventionalMemory
            ReservePages(desc->physAddr, desc->numeroPaginas);
        }
    }
}

void AlocadorDePageFrame::InitBitmap(size_t bitmapSize, void* bufferAddress){
    PageBitmap.Size = bitmapSize;
    PageBitmap.Buffer = (uint8_t*)bufferAddress;
    for (int i = 0; i < bitmapSize; i++){
        *(uint8_t*)(PageBitmap.Buffer + i) = 0;
    }
}
uint64_t pageBitmapIndex = 0;
void* AlocadorDePageFrame::RequisitarPagina(){
    for (; pageBitmapIndex < PageBitmap.Size * 8; pageBitmapIndex++){
        if (PageBitmap[pageBitmapIndex] == true) continue;
        LockPage((void*)(pageBitmapIndex * 4096));
        return (void*)(pageBitmapIndex * 4096);
    }

    return NULL; // Page Frame Swap to file
}

void AlocadorDePageFrame::FreePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == false) return;
    if (PageBitmap.Set(index, false)){
        freeMemory += 4096;
        usedMemory -= 4096;
        if (pageBitmapIndex > index) pageBitmapIndex = index;
    }
}

void AlocadorDePageFrame::FreePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        FreePage((void*)((uint64_t)address + (t * 4096)));
    }
}

void AlocadorDePageFrame::LockPage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == true) return;
    if (PageBitmap.Set(index, true)){
        freeMemory -= 4096;
        usedMemory += 4096;
    }
}

void AlocadorDePageFrame::LockPages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        LockPage((void*)((uint64_t)address + (t * 4096)));
    }
}

void AlocadorDePageFrame::UnreservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == false) return;
    if (PageBitmap.Set(index, false)){
        freeMemory += 4096;
        reservedMemory -= 4096;
        if (pageBitmapIndex > index) pageBitmapIndex = index;
    }
}

void AlocadorDePageFrame::UnreservePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        UnreservePage((void*)((uint64_t)address + (t * 4096)));
    }
}

void AlocadorDePageFrame::ReservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == true) return;
    if (PageBitmap.Set(index, true)){
        freeMemory -= 4096;
        reservedMemory += 4096;
    }
}

void AlocadorDePageFrame::ReservePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        ReservePage((void*)((uint64_t)address + (t * 4096)));
    }
}

uint64_t AlocadorDePageFrame::GetFreeRAM(){
    return freeMemory;
}
uint64_t AlocadorDePageFrame::GetUsedRAM(){
    return usedMemory;
}
uint64_t AlocadorDePageFrame::GetReservedRAM(){
    return reservedMemory;
}