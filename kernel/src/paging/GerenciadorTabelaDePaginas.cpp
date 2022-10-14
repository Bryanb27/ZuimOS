#include "GerenciadorTabelaDePaginas.h"
#include "PageMapIndexer.h"
#include <stdint.h>
#include "AlocadorDePageFrame.h"
#include "../memory.h"

GerenciadorTabelaDePaginas::GerenciadorTabelaDePaginas(TabelaDePagina* enderecoPML4){
    this->PML4 = enderecoPML4;
}

void GerenciadorTabelaDePaginas::MapMemory(void* memoriaVirtual, void* memoriaFisica){
    PageMapIndexer indexer = PageMapIndexer((uint64_t)memoriaVirtual);
    EntradaDiretorioDePagina PDE;

    PDE = PML4->entries[indexer.PDP_i];
    TabelaDePagina* PDP;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PDP = (TabelaDePagina*)AlocadorGlobal.RequisitarPagina();
        memset(PDP, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PML4->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (TabelaDePagina*)((uint64_t)PDE.GetEndereco() << 12);
    }
    
    
    PDE = PDP->entries[indexer.PD_i];
    TabelaDePagina* PD;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PD = (TabelaDePagina*)AlocadorGlobal.RequisitarPagina();
        memset(PD, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDP->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (TabelaDePagina*)((uint64_t)PDE.GetEndereco() << 12);
    }

    PDE = PD->entries[indexer.PT_i];
    TabelaDePagina* PT;
    if (!PDE.GetFlag(PT_Flag::Present)){
        PT = (TabelaDePagina*)AlocadorGlobal.RequisitarPagina();
        memset(PT, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PD->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (TabelaDePagina*)((uint64_t)PDE.GetEndereco() << 12);
    }

    PDE = PT->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)memoriaFisica >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PT->entries[indexer.P_i] = PDE;
}