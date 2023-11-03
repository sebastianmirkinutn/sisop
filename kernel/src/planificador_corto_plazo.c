#include "planificador_corto_plazo.h"

extern sem_t grado_de_multiprogramacion;
extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_blocked;
extern sem_t procesos_en_new;
extern sem_t procesos_en_ready;
extern sem_t planificacion_largo_plazo;
extern sem_t planificacion_corto_plazo;
  
extern t_queue *cola_ready;
extern t_queue *cola_blocked;
extern t_queue *cola_exit;
extern t_queue *cola_new;
 
extern t_pcb* execute;
extern t_list* recursos_disponibles;
t_motivo_desalojo motivo;

void planificador_rr(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo, "Empieza el planificador fifo");
    pthread_t generador_de_interrupciones;
    while(1)
    {
        sem_wait(&planificacion_corto_plazo);
        sem_wait(&procesos_en_ready);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_ready);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);

        execute = queue_pop(cola_ready);
        sem_post(&mutex_cola_ready);
        execute->estado = EXEC;

        log_info(logger_hilo, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", execute->pid);
  
        send(arg_h->socket_dispatch, &(execute->pid), sizeof(uint32_t), 0);
        log_info(logger_hilo, "Envié %i a %i", execute->pid, arg_h->socket_dispatch);
        enviar_contexto_de_ejecucion(execute->contexto, arg_h->socket_dispatch);

        arg_h->pcb = execute;
        pthread_create(&generador_de_interrupciones, NULL, &clock_interrupt, (void*)arg_h);
        pthread_detach(&generador_de_interrupciones);

        execute->contexto = recibir_contexto_de_ejecucion(arg_h->socket_dispatch);
        motivo = recibir_motivo_desalojo(arg_h->socket_dispatch);
        evaluar_motivo_desalojo(motivo, arg);
        log_info(logger_hilo, "Fin de proceso %i motivo %s (%i)", execute->pid, de_t_motivo_a_string(motivo));

        sem_post(&planificacion_corto_plazo);
    }
}

void planificador_fifo(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo, "Empieza el planificador fifo");
    while(1)
    {
        sem_wait(&planificacion_corto_plazo);
        sem_wait(&procesos_en_ready);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_ready);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);

        execute = queue_pop(cola_ready);
        sem_post(&mutex_cola_ready);
        execute->estado = EXEC;

        log_info(logger_hilo, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", execute->pid);
  
        send(arg_h->socket_dispatch, &(execute->pid), sizeof(uint32_t), 0);
        
        
        enviar_contexto_de_ejecucion(execute->contexto, arg_h->socket_dispatch);
        
        log_info(logger_hilo, "Envié %i a %i", execute->pid, arg_h->socket_dispatch);
        
        execute->contexto = recibir_contexto_de_ejecucion(arg_h->socket_dispatch);
        motivo = recibir_motivo_desalojo(arg_h->socket_dispatch);
        evaluar_motivo_desalojo(motivo, arg);
        log_info(logger_hilo, "Fin de proceso %i motivo %s (%i)", execute->pid, de_t_motivo_a_string(motivo));
        
        sem_post(&planificacion_corto_plazo);
    }
}

void planificador_prioridades(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo, "Empieza el planificador por prioridades");
    op_code operacion;
    char* recurso;
    while(1)
    {
        sem_wait(&planificacion_corto_plazo);
        sem_wait(&procesos_en_ready);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_ready);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);

        ordenar_colas_segun_prioridad(cola_ready);
        execute = queue_pop(cola_ready);
        sem_post(&mutex_cola_ready);
        execute->estado = EXEC;

        log_info(logger_hilo, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", execute->pid);

        send(arg_h->socket_dispatch, &(execute->pid), sizeof(uint32_t), 0);
        log_info(logger_hilo, "Envié %i a %i", execute->pid, arg_h->socket_dispatch);
        enviar_contexto_de_ejecucion(execute->contexto, arg_h->socket_dispatch);

        execute->contexto = recibir_contexto_de_ejecucion(arg_h->socket_dispatch);
        motivo = recibir_motivo_desalojo(arg_h->socket_dispatch);
        evaluar_motivo_desalojo(motivo, arg);
        log_info(logger_hilo, "Fin de proceso %i motivo %s (%i)", execute->pid, de_t_motivo_a_string(motivo));

        sem_post(&planificacion_corto_plazo);
    }
}

void clock_interrupt(void* arg)
{
    t_log* logger_hilo_ci = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo_ci, "Empieza clock_interrupt");
    op_code operacion = INTERRUPT;
    sleep(arg_h->quantum / 1000);
    if (arg_h->pcb->estado == EXEC){
        printf("Mando interrupcion por el proceso %i", arg_h->pcb->pid);
        send(arg_h->socket_interrupt, &operacion, sizeof(op_code), 0);
    }
}

void ordenar_colas_segun_prioridad(t_queue* queue)
{
    bool comparar_prioridad(void* arg1, void* arg2)
    {
        t_pcb* pcb1 = (t_pcb*) arg1;
        t_pcb* pcb2 = (t_pcb*) arg2;
        return (pcb1->prioridad <= pcb2->prioridad);
    }
    list_sort(queue->elements, (bool*)&comparar_prioridad);
}

char* de_t_motivo_a_string(t_motivo_desalojo motivo)
{
    switch (motivo)
    {
    case SUCCESS:
        return "SUCCESS";
        break;

    case INVALID_RESOURCE:
        return "INVALID_RESOURCE";
        break;

    case CLOCK_INTERRUPT:
        return "CLOCK_INTERRUPT";
        break;

    case INVALID_WRITE:
        return "INVALID_WRITE";
        break;

    default:
        return "OTRO";
        break;
    }
}

void evaluar_motivo_desalojo(t_motivo_desalojo motivo, void* arg)
{
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    switch (motivo)
    {
    case SUCCESS:
        execute->estado = EXIT;
        /* AGREGO A COLA_EXIT */
        break;

    case CLOCK_INTERRUPT:
        execute->estado = READY;
        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, execute);
        sem_post(&mutex_cola_ready);
        sem_post(&procesos_en_ready);
        break;

    case WAIT:
        printf("Me pidieron WAIT\n");
        char* recurso = recibir_mensaje(arg_h->socket_dispatch);
        printf("Me pidieron WAIT de %s\n", recurso);
        wait_recurso(recurso, arg_h->socket_dispatch);
        break;
    case SIGNAL:
        recurso = recibir_mensaje(arg_h->socket_dispatch);
        signal_recurso(recurso, arg_h->socket_dispatch);
        break;
    case F_OPEN:
        enviar_operacion(arg_h->socket_filesystem, F_OPEN);
        break;
    
    default:
        break;
    }
}

void wait_recurso(char* recurso_buscado, int socket_cpu_dispatch)
{
    printf("FUNCIÓN WAIT\n");
    t_recurso* recurso = NULL;
    bool buscar_recurso(void* arg)
    {
        t_recurso* elemento = (t_recurso*) arg;
        return (!strcmp(elemento->nombre, recurso_buscado));
    }
    recurso = list_find(recursos_disponibles, buscar_recurso);
    printf("recurso = %s\n", recurso->nombre);
    if(recurso == NULL)
    {
        /*El recurso no existe*/
        printf("El recurso %s no existe\n", recurso_buscado);
        sem_wait(&mutex_cola_blocked);
        queue_push(&cola_blocked, execute);
        sem_post(&mutex_cola_blocked);
    }
    else
    {
        printf("El recurso existe\n");
        if(recurso->instancias > 0)
        {
            recurso->instancias--;
            printf("Entro al list_find\n");
            recurso = (t_recurso*) list_find(execute->recursos_asignados, buscar_recurso);
            printf("Pasé el list_find\n");
            if(recurso != NULL)
            {
                recurso->instancias++;
                printf("se asigna el recurso %s\n", recurso->nombre);
            }
            else
            {
                t_recurso* recurso = crear_recurso(recurso_buscado, 1); //Creo el recurso con una instancia porque es la instancia que le asigno
                list_add(execute->recursos_asignados, (void*) recurso);
                printf("se asigna el recurso %s\n", recurso->nombre);
            }
            //El recurso se asigna y se devuelve el contexto de ejecución
            //send(socket_cpu_dispatch, &(execute->pid), sizeof(uint32_t), NULL);
            //enviar_contexto_de_ejecucion(execute->contexto, socket_cpu_dispatch);
            sem_wait(&mutex_cola_ready);
            printf("Hice wait de la cola de ready: %i",cola_ready);
            queue_push(cola_ready, execute); // Debería ser en la primera posición.
            sem_post(&mutex_cola_ready);
            sem_post(&procesos_en_ready);
        }
        else
        {
            //No se asigna el recurso y se agrega a la cola de bloqueados.
            sem_wait(&mutex_cola_blocked);
            queue_push(cola_blocked, execute);
            sem_post(&mutex_cola_blocked);
            printf("no se asigna el recurso %s\n", recurso->nombre);
        }
    }
    printf("Termina wait_recurso\n");
    return;
}

void signal_recurso(char* recurso_buscado, int socket_cpu_dispatch)
{
    printf("FUNCIÓN SIGNAL\n");
    t_recurso* recurso = NULL;
    bool buscar_recurso(void* arg)
    {
        t_recurso* elemento = (t_recurso*) arg;
        return (!strcmp(elemento->nombre, recurso_buscado));
    }
    recurso = (t_recurso*) list_find(execute->recursos_asignados, buscar_recurso);
    if(recurso == NULL)
    {
        /*El recurso no existe*/
        printf("El recurso no está asignado\n");
        sem_wait(&mutex_cola_blocked);
        queue_push(cola_blocked, execute);
        sem_post(&mutex_cola_blocked);
    }
    else
    {
        if(recurso->instancias > 0)
        {
            recurso->instancias--;
            recurso = (t_recurso*) list_find(recursos_disponibles, buscar_recurso);
            if(recurso != NULL)
            {
                recurso->instancias++;
                //Se libera el recurso y se devuelve el contexto de ejecución
                send(socket_cpu_dispatch, &(execute->pid), sizeof(uint32_t), NULL);
                enviar_contexto_de_ejecucion(execute->contexto, socket_cpu_dispatch);
            }
            else
            {
            sem_wait(&mutex_cola_blocked);
            queue_push(cola_blocked, execute);
            sem_post(&mutex_cola_blocked);
            }
        }
        else
        {
            sem_wait(&mutex_cola_blocked);
            queue_push(cola_blocked, execute);
            sem_post(&mutex_cola_blocked);
        }
    
    }
}