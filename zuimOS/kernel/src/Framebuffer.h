#pragma once
#include <stddef.h>

//Estrutura para o framebuffer 
//Pode utilizar essa estrutura para armazenar qualquer informacao do framebuffer para um driver grafico
struct Framebuffer{
    //Endereco base e tamanho do buffer
	void* BaseAddress;
	size_t BufferSize;
	//resolucao horizontal
	unsigned int Width; 
	//resolucao vertica
	unsigned int Height;
	unsigned int PixelsPerScanLine;
};