#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include <stdint.h>
#include <commons/collections/list.h>
typedef struct
{
    uint32_t pagina;
    uint32_t frame;
    uint8_t presencia;
    uint8_t modificado;
    uint32_t posicion_en_swap;
}t_pagina;

typedef struct
{
    uint32_t pid;
    t_list* instrucciones;
	t_list* tabla_de_paginas;
}t_proceso;

#endif