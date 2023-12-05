#include "client_utils.h"

int crear_conexion(t_log *logger, char *ip, char *puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int ret_addrinfo = getaddrinfo(ip, puerto, &hints, &server_info);

	if (ret_addrinfo != 0)
	{
		log_error(logger, "Error al obtener información del servidor.");
		return 0;
	}

	int socket_cliente = socket(server_info->ai_family,
								server_info->ai_socktype,
								server_info->ai_protocol);
	if (socket_cliente == -1)
	{
		log_info(logger, "Error al crear el socket.");
		freeaddrinfo(server_info);
		return 0;
	}

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
	{
		log_error(logger, "Error al conectarse al servidor.");
		freeaddrinfo(server_info);
		return 0;
	}

	log_info(logger, "Se estableció la conexión al servidor.");
	freeaddrinfo(server_info);
	return socket_cliente;
}

void eliminar_paquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void *serializar_paquete(t_paquete *paquete, uint32_t bytes)
{
	void *magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

void enviar_mensaje(char *mensaje, int socket_cliente)
{
	t_paquete *paquete = crear_paquete(MENSAJE);
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);
	uint32_t bytes = paquete->buffer->size + sizeof(op_code) + sizeof(uint32_t);
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, NULL);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void crear_buffer(t_paquete *paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(op_code operacion)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = operacion;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, uint32_t size)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + size + sizeof(uint32_t));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &size, sizeof(uint32_t));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(uint32_t), valor, size);

	paquete->buffer->size += size + sizeof(uint32_t);
}

void enviar_paquete(t_paquete *paquete, int socket_cliente)
{
	uint32_t bytes = paquete->buffer->size + sizeof(uint32_t) + sizeof(op_code);
	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void enviar_operacion(int socket, op_code operacion)
{
	send(socket, &operacion, sizeof(op_code), 0);
}

void enviar_respuesta(int socket, t_response respuesta)
{
	send(socket, &respuesta, sizeof(t_response), 0);
}

t_paquete *serializar_op_filesystem(t_opfilesystem *valor)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	int bytes = sizeof(valor->operacion) + strlen(valor->longitud_nombre) + 1 + sizeof(uint32_t) * 2;

	paquete->buffer->size = bytes;

	void *stream = malloc(paquete->buffer->size);
	int offset = 0;

	memcpy(stream + offset, (&valor->operacion), sizeof(valor->operacion));
	offset += sizeof(valor->operacion);
	memcpy(stream + offset, (&valor->longitud_nombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, (&valor->nombre_archivo), strlen(valor->longitud_nombre) + 1);
	offset += strlen(valor->longitud_nombre) + 1;
	memcpy(stream + offset, (&valor->puntero), sizeof(uint32_t));

	paquete->buffer->stream = stream;

	return paquete;
}

t_opfilesystem *deserializar_op_filesystem(t_buffer *buffer)
{
	t_opfilesystem *valor = malloc(sizeof(t_opfilesystem));

	void *stream = buffer->stream;

	memcpy(&(valor->operacion), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(valor->longitud_nombre), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	valor->nombre_archivo = malloc(valor->longitud_nombre);
	memcpy(valor->nombre_archivo, stream, valor->longitud_nombre);
	stream += valor->longitud_nombre;
	memcpy(&(valor->puntero), stream, sizeof(uint32_t));

	return valor;
}