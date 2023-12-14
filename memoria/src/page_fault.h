#ifndef PAGE_FAULT_H
#define PAGE_FAULT_H

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

typedef struct
{
    t_proceso* proceso;
    t_pagina* pagina;
}t_algoritmo_response;


t_algoritmo_response* buscar_victima_fifo(void);
t_algoritmo_response* buscar_victima_lru(void);
int32_t buscar_frame_libre();
void* leer_pagina(uint32_t nro_frame);
void escribir_pagina(uint32_t nro_frame, void* a_escribir);
void swap_in(int socket_swap, t_pagina* pagina, uint32_t frame, t_proceso* proceso);
void swap_out(int socket_swap, t_pagina* pagina, uint32_t frame, t_proceso* proceso);

#endif