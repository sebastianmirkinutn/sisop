#include "planificador_mediano_plazo.h"

extern sem_t grado_de_multiprogramacion;
extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_exit;
extern sem_t procesos_en_new;
extern sem_t procesos_en_ready;
extern sem_t planificacion_largo_plazo;
extern sem_t planificacion_corto_plazo;
  
extern t_queue *cola_ready;
extern t_queue *cola_exit;
extern t_queue *cola_new;
 
extern t_pcb* execute;
extern t_list* recursos_disponibles;
t_motivo_desalojo motivo;
extern t_log* logger;

t_recurso* buscar_recurso(char* recurso_buscado)
{
    bool es_el_recurso(void* arg)
    {
        t_recurso* elemento = (t_recurso*) arg;
        return (!strcmp(elemento->nombre, recurso_buscado));
    }
    t_recurso* recurso = list_find(recursos_disponibles, es_el_recurso);
    return recurso;
}

void desbloquear_procesos(char* recurso_buscado)
{
    bool es_el_recurso(void* arg)
    {
        t_recurso* elemento = (t_recurso*) arg;
        return (!strcmp(elemento->nombre, recurso_buscado));
    }

    t_pcb* pcb;
    t_recurso* recurso = buscar_recurso(recurso_buscado);
    t_recurso* recurso_del_proceso;
    if(recurso != NULL)
    {
        sem_wait(&(recurso->mutex_cola_blocked));
        printf("recurso = %i\n",recurso);
        printf("Queue = %i\n",recurso->cola_blocked->elements->elements_count);
        if(list_size(recurso->cola_blocked->elements) > 0)
        {
            pcb = queue_pop(recurso->cola_blocked);
            if(pcb != NULL)
            {
                recurso_del_proceso = list_find(pcb->recursos_asignados, es_el_recurso);
                if(recurso_del_proceso != NULL)
                {
                    recurso_del_proceso->instancias++;
                }
                else
                {
                    recurso_del_proceso = crear_recurso(recurso_buscado, 1); //Creo el recurso con una instancia porque es la instancia que le asigno
                    list_add(execute->recursos_asignados, (void*) recurso_del_proceso); 
                    printf("se asigna el recurso %s\n", recurso_del_proceso->nombre);
                }

                sem_wait(&mutex_cola_ready);
                queue_push(cola_ready,pcb);
                sem_post(&mutex_cola_ready);
                sem_post(&procesos_en_ready);
            }
        }
        else
        {
            //recurso->instancias++;
        }
        sem_post(&(recurso->mutex_cola_blocked));
    }
    else
    {
        /*El recurso no existe*/
    }
}

void wait_recurso(t_log* logger, char* recurso_buscado, int socket_cpu_dispatch)
{
    bool es_el_recurso(void* arg)
    {
        t_recurso* elemento = (t_recurso*) arg;
        return (!strcmp(elemento->nombre, recurso_buscado));
    }
    printf("FUNCIÓN WAIT\n");
    t_recurso* recurso = NULL;
    t_recurso* recurso_del_proceso = NULL;
    recurso = buscar_recurso(recurso_buscado);
    if(recurso == NULL)
    {
        /*El recurso no existe*/
        printf("El recurso %s no existe\n", recurso_buscado);
        sem_wait(&mutex_cola_exit);
        queue_push(cola_exit, execute);
        sem_post(&mutex_cola_exit);
        log_info(logger, "Fin de proceso %i motivo %s", execute->pid, "INVALID_RESOURCE");
    }
    else
    {
        printf("El recurso existe (%i)\n", recurso->instancias);
        recurso->instancias--;
        if(recurso->instancias >= 0)
        {
            printf("Entro al list_find\n");
            recurso_del_proceso = list_find(execute->recursos_asignados, es_el_recurso);
            printf("Pasé el list_find\n");
            if(recurso_del_proceso != NULL)
            {
                recurso_del_proceso->instancias++;
                printf("se asigna el recurso %s\n", recurso_del_proceso->nombre);
            }
            else
            {
                recurso_del_proceso = crear_recurso(recurso_buscado, 1); //Creo el recurso con una instancia porque es la instancia que le asigno
                list_add(execute->recursos_asignados, (void*) recurso_del_proceso); 
                printf("se asigna el recurso %s\n", recurso_del_proceso->nombre);
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
            recurso_del_proceso = list_find(execute->recursos_asignados, es_el_recurso);
            if(recurso_del_proceso == NULL)
            {
                recurso_del_proceso = crear_recurso(recurso_buscado, 0);
                list_add(execute->recursos_asignados, (void*) recurso_del_proceso); 
                printf("se asigna el recurso %s\n", recurso_del_proceso->nombre);
            }
            sem_wait(&(recurso->mutex_cola_blocked));
            queue_push(recurso->cola_blocked, execute);
            sem_post(&(recurso->mutex_cola_blocked));
            printf("no se asigna el recurso %s\n", recurso->nombre);
        }
    }
    printf("Termina wait_recurso\n");
    return;
}

void signal_recurso(t_log* logger, char* recurso_buscado, int socket_cpu_dispatch, t_pcb* execute)
{
    bool es_el_recurso(void* arg)
    {
        t_recurso* elemento = (t_recurso*) arg;
        return (!strcmp(elemento->nombre, recurso_buscado));
    }
    printf("FUNCIÓN SIGNAL\n");
    t_recurso* recurso = NULL;
    t_recurso* recurso_del_proceso = NULL;
    recurso = buscar_recurso(recurso_buscado);
    if(recurso == NULL)
    {
        /*El recurso no existe*/
        printf("El recurso no existe\n");
        sem_wait(&mutex_cola_exit);
        queue_push(cola_exit, execute);
        sem_post(&mutex_cola_exit);
        log_info(logger, "Fin de proceso %i motivo %s", execute->pid, "INVALID_RESOURCE");
    }
    else
    {
        recurso_del_proceso = list_find(execute->recursos_asignados, es_el_recurso);
        if(recurso_del_proceso != NULL)
        {
            if(recurso_del_proceso->instancias >= 1)
            {
                recurso->instancias++;
                recurso_del_proceso->instancias--;
                //Se libera el recurso y se devuelve el contexto de ejecución
                printf("Voy a hacer wait de la cola de ready: %i\n",cola_ready);
                sem_wait(&mutex_cola_ready);
                printf("Hice wait de la cola de ready: %i\n",cola_ready);
                agregar_primero_en_cola(cola_ready, execute); // Debería ser en la primera posición.
                sem_post(&mutex_cola_ready);
                sem_post(&procesos_en_ready);
                desbloquear_procesos(recurso_buscado);
                printf("Se liberó el recurso\n");
            }
            else
            {
                //En este caso el recurso existe, pero nunca se hizo wait.
                printf("El recurso no existe\n");
                sem_wait(&mutex_cola_exit);
                queue_push(cola_exit, execute);
                sem_post(&mutex_cola_exit);
                log_info(logger, "Fin de proceso %i motivo %s", execute->pid, "INVALID_RESOURCE");
            }
        }
        else
        {
            printf("El recurso no existe\n");
            sem_wait(&mutex_cola_exit);
            queue_push(cola_exit, execute);
            sem_post(&mutex_cola_exit);
            log_info(logger, "Fin de proceso %i motivo %s", execute->pid, "INVALID_RESOURCE");
        }
    
    }
}

void atender_page_fault(void *arg)
{
    t_args_hilo_archivos *arg_h = (t_args_hilo_archivos *)arg;
    t_response respuesta;

    enviar_operacion(arg_h->socket_memoria, OP_PAGE_FAULT);
    send(arg_h->socket_filesystem, &(arg_h->execute->pid), sizeof(uint32_t), NULL);
    send(arg_h->socket_filesystem, &(arg_h->pagina), sizeof(uint32_t), NULL);

    respuesta = recibir_respuesta(arg_h->socket_memoria);
    switch (respuesta)
    {
    case OK:
        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, arg_h->execute);
        sem_post(&mutex_cola_ready);
        sem_post(&procesos_en_ready);
        break;
    
    default:
        break;
    }
}