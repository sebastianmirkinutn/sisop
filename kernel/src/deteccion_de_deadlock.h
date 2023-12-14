#ifndef DETECCION_DE_DEADLOCK_H
#define DETECCION_DE_DEADLOCK_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sockets/sockets.h>
#include <sockets/client_utils.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pcb/pcb.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <log/log_utils.h>
#include <pcb/pcb.h>
#include <pthread.h>
#include "archivos.h"

typedef struct
{
    char* nombre;
    uint32_t instancias;
}t_recurso_deadlock;


typedef struct
{
    uint32_t pid;
    t_list* recursos_asignados;
    t_list* solicitudes_actuales; 
}t_proceso_deadlock;


#endif