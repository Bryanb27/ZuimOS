#pragma once
#include "math.h"
#include "Framebuffer.h"
#include "simpleFonts.h" 

class RenderBasico{
    public:
    RenderBasico(Framebuffer* framebufferAlvo, PSF1_FONT* psf1_Font);
    Point PosicaoCursor;
    Framebuffer* FramebufferAlvo;
    PSF1_FONT* PSF1_Font;
    unsigned int Cor;
    void Print(const char* str);
    void PutChar(char chr, unsigned int xOff, unsigned int yOff);
};

extern RenderBasico* RenderizadorGlobal;