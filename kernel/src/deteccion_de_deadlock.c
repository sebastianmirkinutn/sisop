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

t_proceso_deadlock* crear_proceso_deadloc(t_pcb* proceso)
{
    t_proceso_deadlock* proceso_deadlock = malloc(sizeof(t_proceso_deadlock));
    proceso_deadlock->pid = proceso->pid;
    proceso_deadlock->recursos_asignados = cargar_recursos_deadlock(proceso->recursos_asignados);
    //proceso_deadlock->solicitudes_actuales = 
}

void deteccion_de_deadlock()
{
    t_list* recursos_deadlock_disponibles = cargar_recursos_deadlock(recursos_disponibles);
    //Elimino procesos que no tengan recursos asignados
    
    //Me fijo cual puedo terminar y simulo que lo termino (un for, hasta que no quede ninguno o no cambie nada), sumando los recursos a los disponibles

    //Analizo si la lista está vacía
}