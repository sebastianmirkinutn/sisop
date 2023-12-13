#include "planificador_largo_plazo.h"

extern sem_t grado_de_multiprogramacion;
extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_exit;
extern sem_t procesos_en_new;
extern sem_t procesos_en_ready;
extern sem_t procesos_en_exit;
extern sem_t planificacion_largo_plazo;
extern sem_t planificacion_corto_plazo;
extern sem_t start_interrupts;
extern sem_t mutex_flag_p_finished;
  
extern t_queue *cola_ready;
extern t_queue *cola_exit;
extern t_queue *cola_new;
 
extern t_pcb* execute;
extern t_list* recursos_disponibles;
extern t_list* tabla_global_de_archivos;
extern t_log* logger;

void planificador_largo_plazo(void* arg)
{
    t_args_hilo* arg_h = (t_args_hilo*) arg;

    while(1)
    {
        sem_wait(&procesos_en_new);
        sem_wait(&planificacion_largo_plazo);
        sem_wait(&grado_de_multiprogramacion);
        log_info(logger,"Hice wait del gdmp");
        sem_wait(&mutex_cola_new);
        log_info(logger,"Hice wait de la cola de new: %i",cola_new);
        t_pcb* pcb = queue_pop(cola_new);
        sem_post(&mutex_cola_new);
        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, pcb);
        sem_post(&mutex_cola_ready);
        log_info(logger, "PID:%i - Estado:%i", pcb->pid, pcb->estado);
        log_info(logger, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", pcb->pid);

        op_code operacion = INICIAR_PROCESO;
        send(arg_h->socket_memoria, &operacion, sizeof(op_code), 0);
        send(arg_h->socket_memoria, &(pcb->pid), sizeof(int), 0);
        enviar_mensaje(pcb->archivo_de_pseudocodigo, arg_h->socket_memoria); 
        send(arg_h->socket_memoria, &(pcb->size), sizeof(uint32_t), 0);    
        sem_post(&procesos_en_ready);
        sem_post(&planificacion_largo_plazo);
    }
}

void finalizar_procesos_en_exit(void* arg)
{
    t_args_hilo* arg_h = (t_args_hilo*) arg;

    while(1)
    {
        sem_wait(&procesos_en_exit);
        sem_wait(&planificacion_largo_plazo);
        log_info(logger,"Hice wait del gdmp");
        sem_wait(&mutex_cola_exit);
        log_info(logger,"Hice wait de la cola de new: %i",cola_new);
        t_pcb* pcb = queue_pop(cola_exit);
        sem_post(&mutex_cola_exit);
        finalizar_proceso_en_exit(pcb->pid, arg_h->socket_dispatch, arg_h->socket_memoria, pcb);
        sem_post(&grado_de_multiprogramacion);

        sem_post(&planificacion_largo_plazo);
    }
}

void finalizar_proceso_en_exit(uint32_t pid, int socket_cpu_dispatch, int socket_memoria, t_pcb* pcb)
{
    
    void hacer_f_close(void* arg)
    {
        pthread_t h_file_close_deallocate;
        t_args_hilo_archivos* argumentos_file_management = malloc(sizeof(argumentos_file_management));
        //argumentos_file_management->socket_filesystem = socket_filesystem;
        argumentos_file_management->execute = pcb;
        argumentos_file_management->nombre_archivo = ((t_archivo_local*)arg)->archivo->nombre;
        pthread_create(&h_file_close_deallocate, NULL, &cerrar_archivo, (void*)argumentos_file_management);
        pthread_detach(h_file_close_deallocate);
    }
    void hacer_signal(void* arg)
    {
        t_recurso* recurso_local = (t_recurso*)arg;
        t_recurso* recurso = buscar_recurso(((t_recurso*)arg)->nombre);
        for(uint32_t i = recurso_local->instancias; i > 0; i--)
        {
            recurso->instancias++;
            desbloquear_procesos(recurso->nombre);
        }
        printf("Libero %i instancias del recurso %s\n", recurso_local->instancias, recurso_local->nombre);
        recurso_local->instancias = 0;
        //signal_recurso(logger, ((t_recurso*)arg)->nombre, socket_cpu_dispatch, pcb);
    }

    list_iterate(pcb->recursos_asignados, hacer_signal);
    enviar_operacion(socket_memoria, FINALIZAR_PROCESO);
    send(socket_memoria, &(pcb->pid), sizeof(uint32_t), NULL);
}


