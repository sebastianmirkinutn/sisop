
#include "operaciones.h"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

extern uint32_t cant_bloques_total;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern uint32_t retardo_acceso_bloque;
extern uint32_t retardo_acceso_fat;

extern t_list* archivos_abiertos;

extern t_fat* fat;
extern t_log* logger;

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
	fcb->config = config_fcb;
	config_set_value(config_fcb, "NOMBRE_ARCHIVO", nombre);
	config_set_value(config_fcb, "TAMANIO_ARCHIVO", "0");
	config_set_value(config_fcb, "BLOQUE_INICIAL", int_to_string(UINT32_MAX));
	config_save(config_fcb);
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    close(archivo_fcb);
    return 1;
}
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//----- Version Ignacio - Comentado
/*
int32_t agrandar_archivo(t_fcb* archivo, uint32_t size)
{
	uint32_t por_asignar = size - archivo->tam_archivo;
	uint32_t tam_teorico = ceil(archivo->tam_archivo / tam_bloque) * tam_bloque;
	uint32_t bytes_libres = tam_teorico - archivo->tam_archivo;
	
	if(archivo->bloque_inicial == UINT32_MAX)
	{
		archivo->bloque_inicial = obtener_bloque_libre();
		fat->memory_map[archivo->bloque_inicial] = UINT32_MAX;
		uint32_t asignado = MIN(tam_bloque, por_asignar);
		archivo->tam_archivo += asignado;
		printf("pr_asignar = %i\n", por_asignar);
		por_asignar -= asignado;
		printf("Asigné el orimer bloque y me falta %i\n", por_asignar);
		config_set_value(archivo->config, "BLOQUE_INICIAL", int_to_string(archivo->bloque_inicial));
		config_save(archivo->config);

		if(por_asignar == 0)
		{
			return;
		}
		
	}
	if(bytes_libres > 0)
	{
		uint32_t asignado = MIN(bytes_libres, por_asignar);
		bytes_libres -= asignado;
		por_asignar -= asignado;
		archivo->tam_archivo += asignado;
	}
	while(por_asignar > 0)
	{
		uint32_t nuevo_bloque = obtener_bloque_libre();
		fat->memory_map[ultimo_bloque(archivo->bloque_inicial)] = nuevo_bloque;
		fat->memory_map[nuevo_bloque] = UINT32_MAX;

		uint32_t asignado = MIN(tam_bloque, por_asignar);
		archivo->tam_archivo += asignado;
		
		bytes_libres = tam_bloque - asignado;
		por_asignar -= asignado;
	}
	{
		return;
	}
}


uint32_t ultimo_bloque(uint32_t puntero)
{
	uint32_t puntero_sig = puntero;
	while(puntero_sig != UINT32_MAX)
	{
		puntero_sig = fat->memory_map[puntero];
		if(puntero_sig != UINT32_MAX)
		{
			puntero = puntero_sig;
		}
	}
	return puntero;
}

uint32_t penultimo_bloque(uint32_t puntero)
{
	uint32_t puntero_sig = puntero;
	uint32_t puntero_penult = puntero;
	while(puntero_sig != UINT32_MAX)
	{
		puntero_sig = fat->memory_map[puntero];
		if(puntero_sig != UINT32_MAX)
		{
			puntero = puntero_sig;
			puntero_penult = puntero;
		}
	}
	return puntero_penult;
}
*/

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//----- Version Ignacio - Comentado
/*
void achicar_archivo(t_fcb* archivo, uint32_t size)
{
	uint32_t por_quitar = archivo->tam_archivo - size;
	uint32_t tam_teorico = ceil(archivo->tam_archivo / tam_bloque) * tam_bloque;
	uint32_t bytes_libres = tam_teorico - archivo->tam_archivo;

	if(por_quitar< bytes_libres)
	{
		archivo->tam_archivo -= por_quitar;
		return;
	}
	else
	{
		archivo->tam_archivo -= bytes_libres;
		por_quitar -= bytes_libres;
uint32_t ult_bloque = ultimo_bloque(archivo->bloque_inicial);
			uint32_t penult_bloque = penultimo_bloque(archivo->bloque_inicial);
			printf("ult = %i - penult = %i\n", ult_bloque, penult_bloque);
		while(por_quitar > tam_bloque)
		{
			uint32_t ult_bloque = ultimo_bloque(archivo->bloque_inicial);
			uint32_t penult_bloque = penultimo_bloque(archivo->bloque_inicial);
			printf("ult = %i - penult = %i\n", ult_bloque, penult_bloque);
			if(ult_bloque != penult_bloque)
			{
				fat->memory_map[ult_bloque] = 0;
				fat->memory_map[penult_bloque] = UINT32_MAX;
				archivo->tam_archivo -= MIN(tam_bloque, por_quitar);
			}
			else
			{
				fat->memory_map[ult_bloque] = 0;
				archivo->bloque_inicial = UINT32_MAX;
			}
			por_quitar -= tam_bloque;
		}
		if(por_quitar > 0)
		{
			archivo->tam_archivo -= por_quitar;
		}
	}
}
*/
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//----- Version Ignacio - Comentado
/*
int32_t truncar_archivo(char* nombre, uint32_t size)
{
	printf("truncar_archivo\n");
	t_fcb* archivo = buscar_archivo(nombre, archivos_abiertos);
	printf("Encontré al archivo\n");

	if(size > archivo->tam_archivo) //Se amplía
	{
		agrandar_archivo(archivo, size);
		mem_hexdump(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t));
		config_set_value(archivo->config, "TAMANIO_ARCHIVO", int_to_string(archivo->tam_archivo));
		config_save(archivo->config);
		printf("tam_archivo = %s\n", int_to_string(archivo->tam_archivo));
		fsync(fat->file_descriptor);
	}
	else if (size < archivo->tam_archivo) // Valido que no sea igual porque en ese caso no se hace nada
	{
		achicar_archivo(archivo, size);
		mem_hexdump(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t));
		config_set_value(archivo->config, "TAMANIO_ARCHIVO", int_to_string(archivo->tam_archivo));
		config_set_value(archivo->config, "BLOQUE_INICIAL", int_to_string(archivo->bloque_inicial));
		config_save(archivo->config);
	}
	else
	{
		
	}
	
	return 1;

}
*/


//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//---- Version Pablo - Activo sin comentar

int32_t achicar_archivo(t_fcb* archivo, uint32_t size)
{

	//Por enunciado: Reducir el tamaño del archivo...(descartando desde el final del archivo hacia el principio)
	uint32_t ptrBl1	= fat->memory_map[archivo->bloque_inicial];
	uint32_t ptrBl2;
	uint32_t ptrBl3 = archivo->bloque_inicial;

	printf ("TRUNCAR: ACHICAR ARCHIVO\n");
	uint32_t contBlk=0;
	while (ptrBl1 != UINT32_MAX) {
		contBlk++;
		if (contBlk>1) ptrBl3 = ptrBl2;
		ptrBl2 = ptrBl1;
		log_info(logger, "Acceso FAT - Entrada: %i - Valor: %i", ptrBl1, fat->memory_map[ptrBl1]);
		sleep(retardo_acceso_fat / 1000);
		ptrBl1 = fat->memory_map[ptrBl1];
	}
	//contBlk++;
	printf ("CANTIDAD DE BLOQUES ASIGNADOS:%u\n",contBlk);
	if ((size >= (tam_bloque * contBlk)) || (contBlk == 0)) {
		printf ("\n>>> ACHICA EL ARCHIVO PERO NO DESASIGNA BLOQUES\n");
		archivo->tam_archivo = size;
		config_set_value(archivo->config, "TAMANIO_ARCHIVO", int_to_string(archivo->tam_archivo));
		config_save(archivo->config);
	}
	else {
		printf ("\n>>> ACHICA EL ARCHIVO PERO SI DESASIGNA BLOQUES\n");
		fat->memory_map[ptrBl2] = 0;
		fat->memory_map[ptrBl3] = UINT32_MAX;
		achicar_archivo(archivo,size);
	}
	return;
}
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//---- Version Pablo - Activo sin comentar

	
int32_t agrandar_archivo(t_fcb* archivo, uint32_t size)
{
	t_fcb* fcb;
	//Al tamaño requerido truncar le resto el tamaño anterior del archivo que podría ser 0 si el
	//archivo es nuevo o tener una cantidad de bytes ya asignados si fue creado previamente
	if(archivo->bloque_inicial == UINT32_MAX)
	{
		//Caso (1) El archivo no existía y se le asigna el primer bloque
 		archivo->bloque_inicial = obtener_bloque_libre();
		fat->memory_map[archivo->bloque_inicial] = UINT32_MAX;
		//-----------------------------------------------------------------------------
		//-- Actualiza el archivo FCB con los datos del tamaño del archivo y el bloque inicial 
		archivo->tam_archivo = MIN (size,tam_bloque);
		config_set_value(archivo->config, "TAMANIO_ARCHIVO", int_to_string(archivo->tam_archivo));
		config_set_value(archivo->config, "BLOQUE_INICIAL", int_to_string(archivo->bloque_inicial));
		config_save(archivo->config);
		//-----------------------------------------------------------------------------
		//msync(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t), MS_SYNC);
		//mem_hexdump(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t));
		if (size > tam_bloque)
			size=size-tam_bloque;
		else
			size=0;
		//-----------------------------------------------------------------------------
		agrandar_archivo_NuevoBloque(archivo,size);
	}
	else
	{	
		uint32_t ptrBl	= fat->memory_map[archivo->bloque_inicial];
		uint32_t contBlk=0;
		while (ptrBl != UINT32_MAX) {
			contBlk++;
			ptrBl = fat->memory_map[ptrBl];
		}
		contBlk++;
		uint32_t tam_fisico = contBlk * tam_bloque;
		uint32_t frag_arch = tam_fisico - archivo->tam_archivo;
		size -= archivo->tam_archivo;
		if (size > frag_arch) {
			size -= frag_arch;
			archivo->tam_archivo += frag_arch;
			config_set_value(archivo->config, "TAMANIO_ARCHIVO", int_to_string(archivo->tam_archivo));
			config_save(archivo->config);
			agrandar_archivo_NuevoBloque(archivo,size);
		}
		else {
			archivo->tam_archivo += size;
			config_set_value(archivo->config, "TAMANIO_ARCHIVO", int_to_string(archivo->tam_archivo));
			config_save(archivo->config);
		}
	}
	
}

int32_t agrandar_archivo_NuevoBloque(t_fcb* archivo, uint32_t size)
{
	if (size>0) {
		//----------------------------------------------------------------------------
		printf ("Adiciona un nuevo bloque a la fat\n");
		uint32_t nuevo_bloque = obtener_bloque_libre();


		log_info(logger, "Acceso FAT - Entrada: %i - Valor: %i", ultimo_bloque(archivo->bloque_inicial), nuevo_bloque);
		sleep(retardo_acceso_fat / 1000);


		fat->memory_map[ultimo_bloque(archivo->bloque_inicial)] = nuevo_bloque;

		

		fat->memory_map[nuevo_bloque] = UINT32_MAX;
		if (size < tam_bloque) 
		{
			archivo->tam_archivo += size;
			size=0;
		}
		else 
		{
			archivo->tam_archivo += tam_bloque;
			size = size - tam_bloque;
		}
		config_set_value(archivo->config, "TAMANIO_ARCHIVO", int_to_string(archivo->tam_archivo));
		config_save(archivo->config);
		agrandar_archivo_NuevoBloque(archivo,size);
	}
	return;
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
		mem_hexdump(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t));
		//msync(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t), MS_SYNC);
		fsync(fat->file_descriptor);
	}
	else if (size < archivo->tam_archivo) // Valido que no sea igual porque en ese caso no se hace nada
	{
		achicar_archivo(archivo, size);
		mem_hexdump(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t));
		config_set_value(archivo->config, "TAMANIO_ARCHIVO", int_to_string(archivo->tam_archivo));
		config_set_value(archivo->config, "BLOQUE_INICIAL", int_to_string(archivo->bloque_inicial));
		config_save(archivo->config);
	}
	
	archivo->tam_archivo = size;
}

uint32_t ultimo_bloque(uint32_t puntero)
{
	uint32_t puntero_sig = puntero;
	while(puntero_sig != UINT32_MAX)
	{
		puntero_sig = fat->memory_map[puntero];
		sleep(retardo_acceso_fat / 1000);
		if(puntero_sig != UINT32_MAX)
		{
			puntero = puntero_sig;
		}
	}
	return puntero;
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
