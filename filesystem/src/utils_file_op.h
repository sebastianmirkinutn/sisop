#ifndef UTILS_FILE_OP_H
#define UTILS_FILE_OP_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct nodo
{
	char nombreArchivo[128];
	uint32_t tamanio_archivo;
    uint32_t bloque_inicial;
    struct nodo *sig;
} nodoL;

typedef nodoL* lista;


char *buscaDatoEnMensaje(char *mensaje,char *valor,int parametro);

#endif