#include "RenderBasico.h"

RenderBasico* RenderizadorGlobal;

RenderBasico::RenderBasico(Framebuffer* framebufferAlvo, PSF1_FONT* psf1_Font)
{
    FramebufferAlvo = framebufferAlvo;
    PSF1_Font = psf1_Font;
    Cor = 0xffffffff;
    PosicaoCursor = {0, 0};
}

void RenderBasico::Print(const char* str)
{
    
    char* chr = (char*)str;
    while(*chr != 0){
        PutChar(*chr, PosicaoCursor.X, PosicaoCursor.Y);
        PosicaoCursor.X+=8;
        if(PosicaoCursor.X + 8 > FramebufferAlvo->Largura)
        {
            PosicaoCursor.X = 0;
            PosicaoCursor.Y += 16;
        }
        chr++;
    }
}

void RenderBasico::PutChar(char chr, unsigned int xOff, unsigned int yOff)
{
    unsigned int* pixPtr = (unsigned int*)FramebufferAlvo->EnderecoBase;
    char* fontPtr = (char*)PSF1_Font->glyphBuffer + (chr * PSF1_Font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * FramebufferAlvo->PixelsPorLinhaEscaneada)) = Cor;
                }

        }
        fontPtr++;
    }
}