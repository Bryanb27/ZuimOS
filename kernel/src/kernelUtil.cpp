#include "kernelUtil.h"
#include "gdt/gdt.h"
#include "interrupts/IDT.h"
#include "interrupts/interrupts.h"

KernelInfo kernelInfo; 
GerenciadorTabelaDePaginas gerenciadorTabelaDePaginas = NULL;
void PrepareMemory(BootInfo* bootInfo){
    uint64_t mMapEntradas = bootInfo->mMapTamanho / bootInfo->mMapDescSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapTamanho, bootInfo->mMapDescSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    gerenciadorTabelaDePaginas = GerenciadorTabelaDePaginas(PML4);

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntradas, bootInfo->mMapDescSize); t+= 0x1000){
        gerenciadorTabelaDePaginas.MapMemory((void*)t, (void*)t);
    }

    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->EnderecoBase;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
    GlobalAllocator.LockPages((void*)fbBase, fbSize/ 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        gerenciadorTabelaDePaginas.MapMemory((void*)t, (void*)t);
    }

    asm ("mov %0, %%cr3" : : "r" (PML4));

    kernelInfo.gerenciadorTabelaDePaginas = &gerenciadorTabelaDePaginas;
}

IDTR idtr;
void PrepareInterrupts(){
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();

    IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + 0xE * sizeof(IDTDescEntry));
    int_PageFault->SetOffset((uint64_t)PageFault_Handler);
    int_PageFault->type_attr = IDT_TA_InterruptGate;
    int_PageFault->selector = 0x08;

    asm ("lidt %0" : : "m" (idtr));
}

RenderBasico r = RenderBasico(NULL, NULL);
KernelInfo InitializeKernel(BootInfo* bootInfo){
    r = RenderBasico(bootInfo->framebuffer, bootInfo->psf1_Font);
    GlobalRenderer = &r;

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);

    PrepareMemory(bootInfo);

    memset(bootInfo->framebuffer->EnderecoBase, 0, bootInfo->framebuffer->BufferSize);

    PrepareInterrupts();

    return kernelInfo;
}