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
void reservar_bloques_swap(uint32_t cantidad, uint32_t pid, t_log* logger_hilo);
bool proceso_esta_reservado(uint32_t pid);
uint32_t reservar_bloque_swap(uint32_t pid, t_log* logger_hilo);
uint32_t elegir_bloque_swap(t_log* logger_hilo);
void leer_bloque_swap(uint32_t bloque);
void escribir_bloque_swap(uint32_t bloque, void* contenido);
uint32_t escribir_nuevo_bloque_swap(void* contenido, uint32_t pid, t_log* logger_hilo);
void eliminar_en_swap(uint32_t pid);

#endif