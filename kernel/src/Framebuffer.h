#pragma once
#include <stddef.h>

struct Framebuffer{
	void* EnderecoBase;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPorLinhaEscaneada;
};
