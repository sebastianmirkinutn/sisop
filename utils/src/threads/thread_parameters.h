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
    uint32_t quantum;
    t_pcb* pcb;
    int retardo_memoria;
}t_args_hilo;

#endif