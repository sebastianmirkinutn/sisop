#ifndef THREAD_PARMETERS
#define THREAD_PARMETERS

#include <pcb/pcb.h>
#include <locks/lock.h>
#include <memoria/memoria.h>

typedef struct
{
    int socket_dispatch;
    int socket_interrupt;
    int socket_memoria;
    int socket_kernel;
    int socket_cpu;
    int socket_filesystem;
    int socket_swap;
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
    int socket_swap;
    t_pcb* pcb;
    uint32_t quantum;
    uint32_t retardo_memoria;
    uint32_t tam_archivo;
    uint32_t puntero;
    t_lock lock;
    char* nombre_archivo;
    t_log* logger;
    t_direccion_fisica* direccion;
    t_pcb* execute;
    uint32_t pagina;
}t_args_hilo_archivos;

t_args_hilo_archivos* crear_parametros(t_args_hilo* src, char* nombre_archivo, t_log* logger);
void liberar_parametros(t_args_hilo_archivos* arg);

#endif