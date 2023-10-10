#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <log/log_utils.h>
#include <sockets/sockets.h>
#include <sockets/client_utils.h>
#include <sockets/server_utils.h>
#include <pcb/pcb.h>

typedef struct
{
    char* parametro;
}t_parametro;

typedef struct
{
    char* operacion;
    t_list* parametros;
}t_instruccion;


typedef struct
{
    uint32_t pid;
    t_list* instrucciones;
}t_proceso;


#endif