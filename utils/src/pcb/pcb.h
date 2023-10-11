#ifndef PCB_H
#define PCB_H
#include <stdlib.h>
#include <commons/collections/list.h>
#include <ctype.h>
#include <stdint.h>
#include <stddef.h>
#include <commons/collections/list.h>
#include <sockets/sockets.h>
#include <sockets/client_utils.h>
#include <sockets/server_utils.h>

typedef struct
{
	uint32_t AX;
	uint32_t BX;
	uint32_t CX;
	uint32_t DX;
} t_registros;

/*
typedef struct
{
	t_registros registros;
	t_list* instrucciones;
	
} t_contexto_de_ejecucion;
*/

typedef enum
{
	NEW,
	READY,
	EXEC,
	BLOCKED,
	EXIT
} t_estado;

typedef struct
{
	uint32_t pid;
	uint32_t program_counter;
	uint32_t prioridad;
	t_registros* contexto;
	t_list* tabla_de_archivos_abiertos;
	t_estado estado;
	uint32_t cant_instrucciones;
	uint32_t size;
	char* archivo_de_pseudocodigo;
} t_pcb;

t_pcb* crear_pcb(uint32_t prioridad, char* pseudocodigo);
void liberar_pcb(t_pcb* pcb);
void* serializar_contexto(t_registros* registros);
t_registros* deserializar_contexto(void* magic);
t_registros* recibir_contexto_de_ejecucion(int socket);
void enviar_contexto(t_registros* registros, int socket);

#endif