#ifndef THREAD_PARMETERS
#define THREAD_PARMETERS

#include <pcb/pcb.h>
#include <locks/lock.h>

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
}t_args_hilo;

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
    t_lock lock;
    char* nombre_archivo;
}t_args_hilo_archivos;

t_args_hilo_archivos* crear_parametros(t_args_hilo* src, char* nombre_archivo);
void liberar_parametros(t_args_hilo_archivos* arg);

#endif