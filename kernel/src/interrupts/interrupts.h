#pragma once
#include "../RenderBasico.h"

struct interrupt_frame;
__attribute__((interrupt)) void Manipulador_FalhaDePagina(struct interrupt_frame* frame);