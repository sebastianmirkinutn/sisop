#ifndef SWAP_H
#define SWAP_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <log/log_utils.h>
#include <sockets/sockets.h>
#include <sockets/client_utils.h>
#include <sockets/server_utils.h>
#include <pthread.h>
#include <commons/bitarray.h>
#include <threads/thread_parameters.h>

typedef struct{
    uint32_t bloque;
    uint32_t pid;
}t_bloque_swap_info;

void conexion_memoria(void* arg);
void reservar_bloques_swap(uint32_t cantidad, uint32_t pid);
uint32_t reservar_bloque_swap(uint32_t pid);
uint32_t elegir_bloque_swap();
void* leer_swap(uint32_t bloque);
void escribir_swap(uint32_t bloque, void* contenido);
void eliminar_swap(int32_t pid);

#endif