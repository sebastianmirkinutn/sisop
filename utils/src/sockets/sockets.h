#ifndef SOCKETS_H
#define SOCKETS_H

typedef enum
{   MENSAJE,
	PAQUETE,
	INICIAR_PROCESO,
	FETCH_INSTRUCCION,
	INTERRUPT,
	WAIT,
	SIGNAL,
	ASIGNADO,
	NO_ASIGNADO,
	DESALOJO,
	LIBERADO,
	NO_LIBERADO,
	F_READ,
	F_WRITE,
	F_SEEK,
	F_CLOSE,
	F_OPEN
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

#endif