#ifndef SOCKETS_H
#define SOCKETS_H

typedef enum
{   MENSAJE,
	PAQUETE,
	INICIAR_PROCESO,
	FETCH_INSTRUCCION,
	INTERRUPT,
	PEDIDO_DE_FRAME,
	FRAME,
	FINALIZAR_PROCESO
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