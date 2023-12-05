#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "sockets.h"
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int crear_conexion_al_server(t_log *logger, char *ip, char *puerto);
void liberar_conexion(int socket_cliente);
void *serializar_paquete(t_paquete* paquete, uint32_t bytes);
void enviar_mensaje(char *mensaje, int socket_cliente);
void crear_buffer(t_paquete *paquete);
t_paquete* crear_paquete(op_code operacion);
void agregar_a_paquete(t_paquete *paquete, void *valor, uint32_t size);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void eliminar_paquete(t_paquete *paquete);
void enviar_operacion (int socket, op_code operacion);
void enviar_respuesta (int socket, t_response respuesta);
t_opfilesystem* deserializar_op_filesystem(t_buffer* buffer);
t_paquete* serializar_op_filesystem(t_opfilesystem* valor);

#endif