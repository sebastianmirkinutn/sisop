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

extern t_log* logger;

extern sem_t* grado_de_multiprogramacion;
extern sem_t* mutex_cola_new;
extern sem_t* mutex_cola_ready;

extern t_queue* cola_new;
extern t_queue* cola_ready;

void planificador_de_largo_plazo();

#endif 