#ifndef PCB_H
#define PCB_H
#include <commons/collections/list.h>
#include <ctype.h>
#include <stdint.h>

typedef struct
{
	uint32_t AX;
	uint32_t BX;
	uint32_t CX;
	uint32_t DX;
} t_registros;

typedef struct
{
	t_registros registros;
	t_list* instrucciones;
	uint32_t program_counter;
} t_contexto_de_ejecucion;

typedef struct
{
	uint32_t pid;
	uint32_t prioridad;
	t_contexto_de_ejecucion* contexto;
	t_list* tabla_de_archivos_abiertos;
} t_pcb;

t_list* parsear_instrucciones(char* str);
t_contexto_de_ejecucion* crear_contexto_de_ejecucion(char* instrucciones);
t_pcb* crear_pcb(char* instrucciones, uint32_t prioridad);

#endif