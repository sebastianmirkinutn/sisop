#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sockets/client_utils.c>
#include <log/log_utils.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pcb/pcb.h>
#include <semaphore.h>
#include <commons/collections/queue.h>

t_log* logger;

sem_t grado_de_multiprogramacion;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;

//t_queue* cola_new;
//t_queue* cola_ready;

#endif 