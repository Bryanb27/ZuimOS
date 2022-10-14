#include "kernelUtil.h"

extern "C" void _start(BootInfo* bootInfo){

    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    GerenciadorTabelaDePaginas* gerenciadorTabelaDePaginas = kernelInfo.gerenciadorTabelaDePaginas;

    GlobalRenderer->Print("Kernel carregado mano");

    int* test = (int*)0x80000000000;
    *test = 2;

    while(true);
}