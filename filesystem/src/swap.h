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
#include <commons/collections/list.h>

void conexion_memoria(void* arg);
t_list* reservar_bloques_swap(uint32_t cantidad);
uint32_t reservar_bloque_swap();
uint32_t buscar_bloque_swap_libre();
void* leer_bloque_swap(uint32_t nro_bloque);
void escribir_bloque_swap(uint32_t nro_bloque, void* contenido);

#endif