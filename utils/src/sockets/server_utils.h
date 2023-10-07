#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "sockets.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include <commons/config.h>
#include <stddef.h>

int iniciar_servidor(t_log* logger, char* puerto);
int esperar_cliente(t_log* logger, int socket_servidor);
int recibir_operacion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
t_paquete* recibir_paquete(t_log* logger, int conexion_socket);
void terminar_programa(t_log* logger, t_config* config);

#endif