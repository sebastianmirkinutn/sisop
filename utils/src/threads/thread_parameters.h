#ifndef THREAD_PARMETERS
#define THREAD_PARMETERS

typedef struct
{
    int socket_dispatch;
    int socket_interrupt;
    int socket_memoria;
    uint32_t quantum;
    t_pcb* pcb;
}t_args_hilo;

#endif