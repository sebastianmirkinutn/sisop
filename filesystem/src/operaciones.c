
#include "operaciones.h"

extern uint32_t cant_bloques_total;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern uint32_t retardo_acceso_bloque;
extern uint32_t retardo_acceso_fat;

extern t_list* archivos_abiertos;

extern t_fat* fat;

int32_t abrir_archivo(char* path_fcb, char* nombre)
{
	char* ruta = malloc(strlen(path_fcb) + 1 + strlen(nombre) + 4 + 1);
	strcpy(ruta, path_fcb);
	uint32_t tam_archivo;
	strcat(ruta, "/");
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	int archivo_fcb = open(ruta, O_RDONLY);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    close(archivo_fcb);
    t_fcb* fcb = leer_fcb(path_fcb, nombre);
	list_add(archivos_abiertos, fcb);
	tam_archivo = fcb->tam_archivo;
	liberar_fcb(fcb);
	printf("aa_tam_archivo = %i\n", tam_archivo);
    return tam_archivo;
}

uint32_t crear_archivo(char* path_fcb, char* nombre)
{
    t_fcb* fcb = crear_fcb(nombre);
	char* ruta = malloc(strlen(path_fcb) + 1 + strlen(nombre) + 4 + 1);
	strcpy(ruta, path_fcb);
	uint32_t tam_archivo;
	strcat(ruta, "/");
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	printf("PATH: %s", ruta);
	int archivo_fcb = open(ruta, O_CREAT | O_RDWR, 0664);
	t_config* config_fcb = iniciar_config(ruta);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	list_add(archivos_abiertos, fcb);
	config_set_value(config_fcb, "NOMBRE_ARCHIVO", nombre);
	config_set_value(config_fcb, "TAMANIO_ARCHIVO", "0");
	config_set_value(config_fcb, "BLOQUE_INICIAL", int_to_string(UINT32_MAX));
	config_save(config_fcb);
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    close(archivo_fcb);
    return 1;
}

int32_t agrandar_archivo(t_fcb* archivo, uint32_t size)
{
	uint32_t por_asignar = archivo->tam_archivo - size;
	uint32_t tam_teorico = ceil(archivo->tam_archivo / tam_bloque) * tam_bloque;
	uint32_t bytes_libres = tam_teorico - archivo->tam_archivo;

	t_fcb* fcb;
	
	if(archivo->bloque_inicial == UINT32_MAX)
	{
		archivo->bloque_inicial = obtener_bloque_libre();
		fat->memory_map[archivo->bloque_inicial] = UINT32_MAX;
		por_asignar -= tam_bloque;
		archivo->tam_archivo += tam_bloque;
		if(por_asignar <= 0)
		{
			return;
		}
	}
	if(bytes_libres > 0)
	{
		/*
		por_asignar -= tam_bloque - bytes_libres;
		archivo->tam_archivo += bytes_libres;
		if(por_asignar > 0)
		{
			agrandar_archivo(archivo, por_asignar);
		}
		else
		{
			return;
		}
		*/
	}
}

int32_t truncar_archivo(char* nombre, uint32_t size)
{
	printf("truncar_archivo\n");
	t_fcb* archivo = buscar_archivo(nombre, archivos_abiertos);
	printf("Encontré al archivo\n");
	//archivo->tam_archivo = size; //Faltan validaciones
	if(size > archivo->tam_archivo) //Se amplía
	{
		agrandar_archivo(archivo, size);
	}
	else if (size < archivo->tam_archivo) // Valido que no sea igual porque en ese caso no se hace nada
	{

	}
}

