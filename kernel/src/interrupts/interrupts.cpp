#include "interrupts.h"

__attribute__((interrupt)) void Manipulador_FalhaDePagina(struct interrupt_frame* frame){
    RenderizadorGlobal->Print("Falha de pagina detectada");
    while(true);
}