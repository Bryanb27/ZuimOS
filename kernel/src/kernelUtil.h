#pragma once

#include <stdint.h>
#include "RenderBasico.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memory.h"
#include "Bitmap.h"
#include "paging/AlocadorDePageFrame.h"
#include "paging/PageMapIndexer.h"
#include "paging/paging.h"
#include "paging/GerenciadorTabelaDePaginas.h"

struct BootInfo {
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapTamanho;
	uint64_t mMapTamanhoDesc;
} ;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct KernelInfo {
    GerenciadorTabelaDePaginas* gerenciadorTabelaDePaginas;
};

KernelInfo IniciarKernel(BootInfo* BootInfo);