#ifndef PLANIFICADOR_CORTO_PLAZO_H
#define PLANIFICADOR_CORTO_PLAZO_H

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
#include <memoria/memoria.h>

#include "manejo_de_archivos.h"

char* de_t_motivo_a_string(t_motivo_desalojo motivo);
void evaluar_motivo_desalojo(t_log* logger_hilo, t_motivo_desalojo motivo, void* arg);
void clock_interrupt(void* arg);
void planificador_rr(void* arg);
void planificador_fifo(void* arg);
t_recurso* crear_recurso(char* nombre, uint32_t instancias);
void ordenar_colas_segun_prioridad(t_queue* queue);
void planificador_prioridades(void* arg);

#endif 