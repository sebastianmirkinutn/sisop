#ifndef MEMORIA_DE_USUARIO_H
#define MEMORIA_DE_USUARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <log/log_utils.h>
#include <sockets/sockets.h>
#include <sockets/client_utils.h>
#include <sockets/server_utils.h>
#include <pcb/pcb.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/bitarray.h>
#include <threads/thread_parameters.h>
#include <memoria/memoria.h>
#include <stdint.h>

uint32_t leer_de_memoria(t_direccion_fisica* direccion);
void escribir_en_memoria(t_direccion_fisica* direccion, uint32_t a_escribir);
int32_t obtener_numero_de_marco(uint32_t pid, uint32_t pagina_buscada);
void asignar_memoria(uint32_t pid, uint32_t size, uint32_t (*algoritmo)(void), t_list* bloques_swap);
uint32_t buscar_victima_fifo(void);
uint32_t buscar_victima_lru(void);
t_proceso* buscar_proceso(uint32_t pid);
void conexion_filesystem(void* arg);
int32_t buscar_frame_libre();

//uint32_t ultima_pagina_accedida();

#endif