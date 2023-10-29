#include"utils.h"


t_log* logger;

int iniciar_servidor(char *c_direccion_ip_filesystem,char *puerto_escucha)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado-servidor");

	int socket_servidor;

	struct addrinfo hints, *servinfo;
	//struct addrinfo hints, *servinfo, *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	/*getaddrinfo(NULL, PUERTO, &hints, &servinfo);*/
	getaddrinfo(c_direccion_ip_filesystem, puerto_escucha, &hints, &servinfo);

	socket_servidor = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);

	// Creamos el socket de escucha del servidor

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(socket_servidor, SOMAXCONN);


	// Escuchamos las conexiones entrantes

	freeaddrinfo(servinfo);
	//log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado-cliente!");

	// Aceptamos un nuevo cliente
	int socket_cliente;
	socket_cliente = accept(socket_servidor,NULL,NULL);
	//log_info(logger, "Se conecto un cliente!");
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

int recibir_mensaje(int socket_cliente)
{
	int size;
	int operacion=25;
	char* buffer = recibir_buffer(&size, socket_cliente);
	//log_info(logger, "Me llego el mensaje %s", buffer);
	printf("El mensaje recibido desde el cliente:%s\n",buffer);
	if (strcmp(buffer,"RESET_FILESYSTEM")==0) {
		operacion=1;
	}
	else {
		if (strcmp(buffer,"RESET_FAT")==0) {
			operacion=2;
		}
		else {
			if (strcmp(buffer,"ABRIR_ARCHIVO")==0) {
				operacion=3;
			}
			else {
				if (strcmp(buffer,"CREAR_ARCHIVO")==0) {
					operacion=4;
				}
				else {
					if (strcmp(buffer,"TRUNCAR_ARCHIVO")==0) {
						operacion=5;
					}
					else {
						if (strcmp(buffer,"LEER_ARCHIVO")==0) {
							operacion=6;
						}
						else {
							if (strcmp(buffer,"ESCRIBIR_ARCHIVO")==0) {
								operacion=7;
							}
							else {
								if (strcmp(buffer,"MOSTRAR_TABLA_FAT")==0) {
									operacion=8;
								}
								else {
									if (strcmp(buffer,"FIN_DE_PROGRAMA")==0) {
										operacion=9;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	free(buffer);
	return(operacion);
}

t_list* recibir_paquete(int socket_cliente)
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
