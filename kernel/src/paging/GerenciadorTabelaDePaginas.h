#pragma once
#include "paging.h"

class GerenciadorTabelaDePaginas {
    public:
    GerenciadorTabelaDePaginas(PageTable* PML4Address);
    PageTable* PML4;
    void MapMemory(void* virtualMemory, void* physicalMemory);
};