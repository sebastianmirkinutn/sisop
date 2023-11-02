#include "mmu.h"

int traducir_direccion(char* direccion_logica, uint32_t tam_pagina)
{
    uint32_t pagina = floor(atof(direccion_logica));
    uint32_t desplazamiento = atoi(direccion_logica) - pagina * tam_pagina;

}