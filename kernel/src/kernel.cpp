#include "kernelUtil.h"

extern "C" void _start(BootInfo* bootInfo){

    KernelInfo kernelInfo = IniciarKernel(bootInfo);
    GerenciadorTabelaDePaginas* gerenciadorTabelaDePaginas = kernelInfo.gerenciadorTabelaDePaginas;

    RenderizadorGlobal->Print("Kernel carregado mano");

    int* test = (int*)0x80000000000;
    *test = 2;

    while(true);
}