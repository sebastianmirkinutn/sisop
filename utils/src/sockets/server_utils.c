#include "server_utils.h"

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

	if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	{
		log_error(logger, "Error al marcar la IP y puerto como reusables");
	}

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_info(logger, "Listo para escuchar a mi cliente");
	return socket_servidor;
}

int esperar_cliente(t_log* logger, int socket_servidor)
{
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

op_code recibir_operacion(int socket_cliente)
{
	op_code cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(op_code), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * stream;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	stream = malloc(*size);
	recv(socket_cliente, stream, *size, MSG_WAITALL);

	return stream;
}

t_paquete* desserializar_paquete(void* magic, int bytes)
{
	t_paquete* paquete;
	int desplazamiento = 0;

	memcpy(&(paquete->codigo_operacion), magic + desplazamiento, sizeof(int));
	desplazamiento+= sizeof(int);
	printf("cod_op\n");
	memcpy(&(paquete->buffer->size), magic + desplazamiento, sizeof(int));
	printf("size\n");
	desplazamiento+= sizeof(int);
	memcpy(paquete->buffer->stream, magic + desplazamiento, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return paquete;
}

char* recibir_mensaje(int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), MSG_WAITALL);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket_cliente, paquete->buffer->stream,paquete->buffer->size, MSG_WAITALL);
	char* datos = malloc(paquete->buffer->size);
	memcpy(datos, paquete->buffer->stream,paquete->buffer->size);
	eliminar_paquete(paquete);
	return datos;
}

t_paquete* recibir_paquete(t_log* logger, int socket_cliente)
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