#include "PageMapIndexer.h"

PageMapIndexer::PageMapIndexer(uint64_t enderecoVirtual){
    enderecoVirtual >>= 12;
    P_i = enderecoVirtual & 0x1ff;
    enderecoVirtual >>= 9;
    PT_i = enderecoVirtual & 0x1ff;
    enderecoVirtual >>= 9;
    PD_i = enderecoVirtual & 0x1ff;
    enderecoVirtual >>= 9;
    PDP_i = enderecoVirtual & 0x1ff;
}

