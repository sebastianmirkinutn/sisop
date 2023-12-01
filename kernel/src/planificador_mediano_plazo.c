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
    t_pcb* pcb;
    t_recurso* recurso = buscar_recurso(recurso_buscado);
    if(recurso != NULL)
    {
        sem_wait(&(recurso->mutex_cola_blocked));
        printf("Queue = %i",recurso->cola_blocked);
        if(list_size(recurso->cola_blocked->elements) > 0)
        {
            pcb = queue_pop(recurso->cola_blocked);
        }
        sem_post(&(recurso->mutex_cola_blocked));

        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready,pcb);
        sem_post(&mutex_cola_ready);
    }
    else
    {
        /*El recurso no existe*/
    }
}

void wait_recurso(t_log* logger, char* recurso_buscado, int socket_cpu_dispatch)
{
    printf("FUNCIÓN WAIT\n");
    t_recurso* recurso = NULL;
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
        printf("El recurso existe\n");
        if(recurso->instancias > 0)
        {
            recurso->instancias--;
            printf("Entro al list_find\n");
            recurso = buscar_recurso(recurso_buscado);
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
            agregar_primero_en_cola(cola_ready, execute); // Debería ser en la primera posición.
            sem_post(&mutex_cola_ready);
            sem_post(&procesos_en_ready);
        }
        else
        {
            //No se asigna el recurso y se agrega a la cola de bloqueados.
            sem_wait(&(recurso->mutex_cola_blocked));
            queue_push(recurso->cola_blocked, execute);
            sem_post(&(recurso->mutex_cola_blocked));
            printf("no se asigna el recurso %s\n", recurso->nombre);
        }
    }
    printf("Termina wait_recurso\n");
    return;
}

void signal_recurso(t_log* logger, char* recurso_buscado, int socket_cpu_dispatch)
{
    printf("FUNCIÓN SIGNAL\n");
    t_recurso* recurso = NULL;
    recurso = buscar_recurso(recurso_buscado);
    if(recurso == NULL)
    {
        /*El recurso no existe*/
        printf("El recurso no está asignado\n");
        sem_wait(&(recurso->mutex_cola_blocked));
        queue_push(recurso->cola_blocked, execute);
        sem_post(&(recurso->mutex_cola_blocked));
    }
    else
    {
        if(recurso->instancias > 0)
        {
            recurso->instancias--;
            recurso = buscar_recurso(recurso_buscado);
            if(recurso != NULL)
            {
                recurso->instancias++;
                //Se libera el recurso y se devuelve el contexto de ejecución
                sem_wait(&mutex_cola_ready);
                printf("Hice wait de la cola de ready: %i",cola_ready);
                agregar_primero_en_cola(cola_ready, execute); // Debería ser en la primera posición.
                sem_post(&mutex_cola_ready);
                sem_post(&procesos_en_ready);
                desbloquear_procesos(recurso_buscado);
                printf("Se liberó el recurso\n");
            }
            else
            {
            sem_wait(&(recurso->mutex_cola_blocked));
            queue_push(recurso->cola_blocked, execute);
            sem_post(&(recurso->mutex_cola_blocked));
            }
        }
        else
        {
            sem_wait(&(recurso->mutex_cola_blocked));
            queue_push(recurso->cola_blocked, execute);
            sem_post(&(recurso->mutex_cola_blocked));
        }
    
    }
}