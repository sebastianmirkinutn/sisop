#ifndef PCB_H
#define PCB_H
#include <commons/collections/list.h>
#include <ctype.h>
#include <stdint.h>
#include <stddef.h>

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
} t_pcb;

t_list* parsear_instrucciones(char* str);
t_pcb* crear_pcb(uint32_t prioridad);

#endif