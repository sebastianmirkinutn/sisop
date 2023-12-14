#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <locks/lock.h>

typedef struct
{
    char* nombre;
    uint32_t tam_archivo;
    t_lock lock;
    t_queue* cola_blocked;
    t_list* locks_lectura;
    sem_t mutex_cola_blocked;
    sem_t mutex_locks_lectura;
    uint32_t contador_aperturas;
}t_archivo;

typedef struct
{
    t_archivo* archivo;
    uint32_t puntero;
}t_archivo_local;

#endif