#ifndef PLANIFICADOR_LARGO_PLAZO_H
#define PLANIFICADOR_LARGO_PLAZO_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <log/log_utils.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pcb/pcb.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <threads/thread_parameters.h>
#include <pthread.h>

#include "manejo_de_archivos.h"

void planificador_largo_plazo(void* arg);
void finalizar_procesos_en_exit(void* arg);
void finalizar_proceso_en_exit(uint32_t pid, int socket_cpu_dispatch, int socket_memoria, t_pcb* pcb);

#endif 