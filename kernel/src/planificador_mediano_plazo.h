#ifndef PLANIFICADOR_MEDIANO_PLAZO_H
#define PLANIFICADOR_MEDIANO_PLAZO_H

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

#include "deteccion_de_deadlock.h"

t_recurso* buscar_recurso(char* recurso_buscado);
void desbloquear_procesos(char* recurso_buscado);
void wait_recurso(t_log* logger, char* recurso_buscado, int conexion_cpu_dispatch);
void signal_recurso(t_log* logger, char* recurso_buscado, int socket_cpu_dispatch, t_pcb* execute);
void atender_page_fault(void *arg);
void sleep_function(void* arg);

#endif