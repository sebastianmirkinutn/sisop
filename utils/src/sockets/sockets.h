#ifndef SOCKETS_H
#define SOCKETS_H

#include <stdint.h>

typedef enum
{  MENSAJE,
	PAQUETE,
	INICIAR_PROCESO,
	FETCH_INSTRUCCION,
	INTERRUPT,
	PEDIDO_DE_FRAME,
	FRAME,
	FINALIZAR_PROCESO,
	PEDIDO_SIZE_PAGINA,
	PEDIDO_LECTURA,
	PEDIDO_ESCRITURA,
	PAGE_FAULT,
	ABRIR_ARCHIVO,
	CREAR_ARCHIVO,
	LEER_ARCHIVO,
	ESCRIBIR_ARCHIVO,
	TRUNCAR_ARCHIVO,
	RESET_FILE_SYSTEM,
	RESET_FAT,
	MOSTRAR_TABLA_FAT,
	FIN_DE_PROGRAMA
}op_code;

typedef enum
{   
	OK,
	ERROR
}t_response;

typedef struct
{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct
{
	uint32_t size_mensaje;
	char* mensaje;
} t_mensaje;

typedef struct
{
	op_code operacion;
	char* nombre_archivo;
	uint32_t longitud_nombre;
	uint32_t puntero;

} t_opfilesystem;
#endif
