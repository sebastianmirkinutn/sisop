#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H
typedef enum
{   MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct
{
	int size_mensaje;
	char* mensaje;
} t_mensaje;

typedef struct
{
	uint32_t AX;
	uint32_t BX;
	uint32_t CX;
	uint32_t DX;
} t_registros;

typedef struct
{
	int size_mensaje;
	char* mensaje;
} t_registros;

typedef struct
{
	t_registros registros;
	t_list* instrucciones;
} t_contexto_de_ejecucion;

typedef struct
{
	uint32_t pid;
	uint32_t program_counter;
	uint32_t priridad;
	t_contexto_de_ejecucion* contexto;
	t_list* tabla_de_archivos_abiertos;
} t_pcb;


#endif