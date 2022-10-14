#pragma once
#include "paging.h"

class GerenciadorTabelaDePaginas {
    public:
    GerenciadorTabelaDePaginas(TabelaDePagina* enderecoPML4);
    TabelaDePagina* PML4;
    void MapMemory(void* memoriaVirtual, void* memoriaFisica);
};