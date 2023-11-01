#ifndef INTERRUPCIONES_H
#define INTERRUPCIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <log/log_utils.h>
#include <sockets/sockets.h>
#include <pcb/pcb.h>
#include <semaphore.h>
#include <pthread.h>
#include <threads/thread_parameters.h>

void recibir_interrupciones(void* arg);
void atender_interrupciones(int socket_kernel_dispatch);

#endif