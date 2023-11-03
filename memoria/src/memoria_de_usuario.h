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

int32_t obtener_numero_de_marco(uint32_t pid, uint32_t pagina_buscada);
void enviar_frame (int socket, int32_t frame);
int32_t recibir_frame (int socket);

#endif