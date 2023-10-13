#include "./../include/planificador_largo_plazo.h"
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <log/log_utils.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pcb/pcb.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
/*
void planificador_de_largo_plazo()
{
    while(1)
    {
        sem_wait(grado_de_multiprogramacion);
        sem_wait(mutex_cola_new);
        t_pcb* pcb = queue_pop(cola_new);
        sem_post(mutex_cola_new);
        sem_wait(mutex_cola_ready);
        queue_push(cola_ready, pcb);
        sem_post(mutex_cola_ready);
        log_info(logger, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", pcb->pid);
    }
}
*/