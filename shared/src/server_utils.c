#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include <commons/log.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>



int iniciar_servidor(t_log *logger, char *puerto){
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);
	//printf("IP: %s - PUERTO: %s\n", IP, puerto);

	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");
	return socket_servidor;
}

/*int iniciar_servidor(char* puerto, t_log *logger)
{
	struct addrinfo hints, *server_info;
	int socket_servidor, s;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	s = getaddrinfo(NULL, puerto, &hints, &server_info);
	if(s!=0){
		exit(EXIT_FAILURE);
	}

	socket_servidor = socket(server_info->ai_family,
	                    server_info->ai_socktype,
	                    server_info->ai_protocol);

	if(socket_servidor == -1){
		close(socket_servidor);
		exit(EXIT_FAILURE);
	}

	if(bind(socket_servidor, server_info->ai_addr, server_info->ai_addrlen) == -1){
		close(socket_servidor);
		exit(EXIT_FAILURE);
	}

	if(listen(socket_servidor, SOMAXCONN) == -1){
		close(socket_servidor);
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(server_info);
	log_info(logger, "Servidor iniciado");

	return socket_servidor;
} 
*/

int esperar_cliente(t_log* logger, int socket_servidor)
{
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(t_log* logger, int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(t_log* logger, int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
