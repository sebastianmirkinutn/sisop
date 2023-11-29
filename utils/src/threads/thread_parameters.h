#ifndef THREAD_PARMETERS
#define THREAD_PARMETERS

#include <pcb/pcb.h>

typedef struct
{
    int socket_dispatch;
    int socket_interrupt;
    int socket_memoria;
    int socket_kernel;
    int socket_cpu;
    int socket_filesystem;
    t_pcb* pcb;
    uint32_t quantum;
    uint32_t retardo_memoria;
    uint32_t tam_archivo;
    uint32_t puntero;
    char* nombre_archivo;
    char* lock;
}t_args_hilo;

#endif