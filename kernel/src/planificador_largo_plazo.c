#include "./../include/planificador_largo_plazo.h"
/*
void planificador_largo_plazo(uint32_t multiprogramacion)
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
        log_info(logger, "%i pasasa a ready", pcb->pid);
    }
}
*/