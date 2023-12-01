#include "thread_parameters.h"

t_args_hilo_archivos* crear_parametros(t_args_hilo* src, char* nombre_archivo, t_log* logger)
{
    t_args_hilo_archivos* arg = malloc(sizeof(t_args_hilo_archivos));
    arg->nombre_archivo = malloc(strlen(nombre_archivo) + 1);
    arg->logger = logger;
    strcpy(arg->nombre_archivo, nombre_archivo);
    arg->socket_memoria = src->socket_memoria;
    arg->socket_cpu = src->socket_cpu;
    arg->socket_dispatch = src->socket_dispatch;
    arg->socket_filesystem = src->socket_filesystem;
    arg->socket_interrupt = src->socket_interrupt;
    arg->socket_kernel = src->socket_kernel;
    printf("Argf FS %i:\n", arg->socket_filesystem);
    return arg;
}

void liberar_parametros(t_args_hilo_archivos* arg)
{
    free(arg->nombre_archivo);
    //free(arg->direccion);
    free(arg);
}