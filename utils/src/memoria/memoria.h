#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>

typedef struct
{
    uint32_t marco;
    uint8_t presencia;
    uint8_t modificado;
    uint32_t posicion_en_swap;
};

#endif