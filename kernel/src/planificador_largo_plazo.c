#include "planificador_largo_plazo.h"

extern sem_t grado_de_multiprogramacion;
extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t procesos_en_new;
extern sem_t procesos_en_ready;
extern sem_t planificacion_largo_plazo;
extern sem_t planificacion_corto_plazo;
extern sem_t start_interrupts;
extern sem_t mutex_flag_p_finished;
  
extern t_queue *cola_ready;
extern t_queue *cola_blocked;
extern t_queue *cola_exit;
extern t_queue *cola_new;
 
extern t_pcb* execute;

void planificador_largo_plazo(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;

    while(1)
    {
        sem_wait(&planificacion_largo_plazo);
        sem_wait(&procesos_en_new);
        sem_wait(&grado_de_multiprogramacion);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_new);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);
        t_pcb* pcb = queue_pop(cola_new);
        sem_post(&mutex_cola_new);
        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, pcb);
        sem_post(&mutex_cola_ready);
        log_info(logger_hilo, "PID:%i - Estado:%i", pcb->pid, pcb->estado);
        log_info(logger_hilo, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", pcb->pid);

        op_code operacion = INICIAR_PROCESO;
        send(arg_h->socket_memoria, &operacion, sizeof(op_code), 0);
        send(arg_h->socket_memoria, &(pcb->pid), sizeof(int), 0);
        enviar_mensaje(pcb->archivo_de_pseudocodigo, arg_h->socket_memoria);     
        sem_post(&procesos_en_ready);
        sem_post(&planificacion_largo_plazo);
    }
}
