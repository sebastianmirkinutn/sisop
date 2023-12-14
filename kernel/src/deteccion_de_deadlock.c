#include "deteccion_de_deadlock.h"

extern sem_t grado_de_multiprogramacion;
extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_exit;
extern sem_t procesos_en_new;
extern sem_t procesos_en_exit;
extern sem_t procesos_en_ready;
extern sem_t planificacion_largo_plazo;
extern sem_t planificacion_corto_plazo;
extern sem_t mutex_file_management;
extern sem_t mutex_tabla_global_de_archivos;
 
extern t_queue *cola_new;
extern t_queue *cola_ready;
extern t_queue *cola_exit;
 
extern t_pcb* execute;
extern t_list* recursos_disponibles;
extern t_list* tabla_global_de_archivos;
extern t_log* logger;

t_recurso_deadlock* crear_recurso_deadlock(char* nombre, uint32_t instancias)
{
    t_recurso_deadlock* recurso = malloc(sizeof(t_recurso_deadlock));
    recurso->nombre = strdup(nombre);
    recurso->instancias = instancias;
    return recurso;
}

t_recurso_deadlock* cargar_recursos_deadlock(t_list* recursos)
{
    t_list* recursos_deadlock = list_create();
    void cargar(t_recurso* recurso)
    {
         t_recurso_deadlock* recurso_deadlock = crear_recurso_deadlock(recurso->nombre, recurso->instancias);
         list_add(recursos_deadlock, recurso_deadlock);
    }
    list_iterate(recursos, cargar);
    return recursos_deadlock;
}

t_queue* buscar_cola_blocked_deadlock(t_list* recursos_disponibles, t_list* tabla_global_de_archivos, uint32_t pid)
{

    t_pcb* pcb = NULL;
    t_list_iterator* recursos_disponibles_iterator = list_iterator_create(recursos_disponibles);
    t_recurso* recurso = NULL;

    while(list_iterator_has_next(recursos_disponibles_iterator))
    {
        recurso = list_iterator_next(recursos_disponibles_iterator);
        pcb = buscar_proceso_segun_pid(pid, recurso->cola_blocked);
        if(pcb != NULL)
        {
            return recurso->cola_blocked;
        }
    }

    if(pcb == NULL)
    {   
        t_archivo* archivo = NULL;
        t_list_iterator* tabla_global_de_archivos_iterator = list_iterator_create(tabla_global_de_archivos);

        while(list_iterator_has_next(tabla_global_de_archivos_iterator))
        {
            
            t_archivo* archivo = list_iterator_next(tabla_global_de_archivos_iterator);
            pcb = buscar_proceso_segun_pid(pid, archivo->cola_blocked);
            if(pcb != NULL)
            {
                return archivo->cola_blocked;
            }
        }
    }

}

t_proceso_deadlock* crear_proceso_deadlock(t_pcb* proceso)
{
    t_proceso_deadlock* proceso_deadlock = malloc(sizeof(t_proceso_deadlock));
    proceso_deadlock->pid = proceso->pid;
    proceso_deadlock->recursos_asignados = cargar_recursos_deadlock(proceso->recursos_asignados);

    void agregar_solicitud_actual_recurso(t_recurso* arg)
    {
        if(arg->cola_blocked == buscar_cola_blocked_deadlock(recursos_disponibles, tabla_global_de_archivos, proceso_deadlock->pid))
        {
            t_recurso_deadlock* recurso_deadlock = crear_recurso_deadlock(arg->nombre, 1);
            list_add(proceso_deadlock->solicitudes_actuales, recurso_deadlock);
        }
    }
    void agregar_solicitud_actual_archivo(t_archivo* arg)
    {
        if(arg->cola_blocked == buscar_cola_blocked_deadlock(recursos_disponibles, tabla_global_de_archivos, proceso_deadlock->pid))
        {
            t_recurso_deadlock* recurso_deadlock = crear_recurso_deadlock(arg->nombre, 1);
            list_add(proceso_deadlock->solicitudes_actuales, recurso_deadlock);
        }
    }
    list_iterate(recursos_disponibles, agregar_solicitud_actual_recurso);
    list_iterate(recursos_disponibles, agregar_solicitud_actual_archivo);
    //proceso_deadlock->solicitudes_actuales = 
}

t_list* cargar_procesos_deadlock()
{
    t_list* procesos_deadlock = list_create();
    list_iterate(cola_ready->elements, crear_proceso_deadlock);
    for(int i = 0; i < list_size(recursos_disponibles); i++)
    {
        t_recurso* recurso = list_get(recursos_disponibles, i);
        for(int j = 0; j < list_size(recurso->cola_blocked->elements); j++)
        {
            t_pcb* proceso = list_get(recurso->cola_blocked->elements, j);
            t_proceso_deadlock* proceso_deadlock = crear_proceso_deadlock(proceso);
            list_add(procesos_deadlock, proceso_deadlock);
        }
    }
    for(int i = 0; i < list_size(tabla_global_de_archivos); i++)
    {
        t_archivo* archivo = list_get(tabla_global_de_archivos, i);
        for(int j = 0; j < list_size(archivo->cola_blocked->elements); j++)
        {
            t_pcb* proceso = list_get(archivo->cola_blocked->elements, j);
            t_proceso_deadlock* proceso_deadlock = crear_proceso_deadlock(proceso);
            list_add(procesos_deadlock, proceso_deadlock);
        }
    }
    return procesos_deadlock;
}

void deteccion_de_deadlock()
{
    t_list* recursos_deadlock_disponibles = cargar_recursos_deadlock(recursos_disponibles);
    t_list* procesos_deadlock = cargar_procesos_deadlock();

    bool el_recurso_tiene_cero_instancias(t_recurso_deadlock* recurso_deadlock)
    {
        return recurso_deadlock->instancias == 0;
    }

    bool no_tiene_recursos(t_proceso_deadlock* proceso_deadlock)
    {
        return list_all_satisfy(proceso_deadlock->recursos_asignados, el_recurso_tiene_cero_instancias);
    }

    bool alcanzan_los_recursos_disponibles(t_recurso_deadlock* recurso_deadlock)
    {
        
        return recurso_deadlock->instancias <= l
    }

    bool puedo_terminar_el_proceso(t_proceso_deadlock* proceso_deadlock)
    {
        return recursos_deadlock_disponibles >= procesos_deadlock->solicitudes_actuales;
    }

    void terminar_proceso_si_se_puede(t_proceso_deadlock* proceso_deadlock)
    {

    }

    //Elimino procesos que no tengan recursos asignados
    list_remove_and_destroy_all_by_condition(procesos_deadlock, no_tiene_recursos, free);
    
    //Me fijo cual puedo terminar y simulo que lo termino (un for, hasta que no quede ninguno o no cambie nada), sumando los recursos a los disponibles
    list_iterate(procesos_deadlock, terminar_proceso_si_se_puede);
    //Analizo si la lista está vacía
}