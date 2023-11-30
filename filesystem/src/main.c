#include "main.h"

uint32_t cant_bloques_total;
uint32_t cant_bloques_swap;
uint32_t tam_bloque;
uint32_t retardo_acceso_bloque;
uint32_t retardo_acceso_fat;

t_list* archivos_abiertos; //Si bien la tabla global de archivos abiertos está en kernel, necesitamos guardar una lista de fcbs.
t_fat* fat;

int main(int argc, char* argv[]) {

	uint32_t ui32_tam_de_archivo=0;
    uint32_t ui32_max_entradas_fat=0;
    uint32_t ui32_cant_bloques_total=0;
	uint32_t ui32_cant_bloques_swap=0;
	uint32_t ui32_tam_bloque=0;

	//Usados para operaciones de lectura y escritura
	char buffer_data[1024+1]="";   
	char valorParametro[128]="";

	//------------------------------------------------------------------------
    t_log* logger = iniciar_logger("log_filesystem.log","FILESYSTEM");
    t_config* config = iniciar_config("./cfg/filesystem.config");
    
	char* puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    char* ip_memoria = config_get_string_value(config,"IP_MEMORIA");
    char* puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	char* path_fat = config_get_string_value(config,"PATH_FAT");
    char* path_bloques = config_get_string_value(config,"PATH_BLOQUES");
	char* path_fcb = config_get_string_value(config,"PATH_FCB");
	cant_bloques_total=config_get_int_value(config,"CANT_BLOQUES_TOTAL");
    cant_bloques_swap=config_get_int_value(config,"CANT_BLOQUES_SWAP");
    tam_bloque=config_get_int_value(config,"TAM_BLOQUE");
    retardo_acceso_bloque=config_get_int_value(config,"RETARDO_ACCESO_BLOQUE");
    retardo_acceso_fat=config_get_int_value(config,"RETARDO_ACCESO_FAT");

    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha);
	fat = crear_fat_mapeada(path_fat, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t));
	mem_hexdump(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t));
    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);
    int socket_servidor = iniciar_servidor(logger, puerto_escucha);
    int socket_kernel = esperar_cliente(logger, socket_servidor);
    if(socket_kernel){
        log_info(logger,"Se conectó kernel");
    }

    op_code operacion;
	char* nombre_archivo;
	uint32_t tam_archivo;

	archivos_abiertos = list_create();
	
	while(1)
	{
		operacion = recibir_operacion(socket_kernel);
		printf ("\nSe recibe operacion:%d\n",operacion);
		switch (operacion)
		{
		case ABRIR_ARCHIVO:
			nombre_archivo = recibir_mensaje(socket_kernel);
			{ //Creo un bloque de código porque creo la variable tam_archivo (int32_t) con el mismo nombre que tam_archivo (uint32_t)
				int32_t tam_archivo = abrir_archivo(path_fcb, nombre_archivo);
				printf("m_tam_archivo = %i\n", tam_archivo);
				send(socket_kernel, &tam_archivo, sizeof(int32_t), NULL);
			}
			break;

		case CREAR_ARCHIVO:
			nombre_archivo = recibir_mensaje(socket_kernel);
			printf("Recibí el nombre del archivo\n");
			if(crear_archivo(path_fcb, nombre_archivo))
			{
				enviar_respuesta(socket_kernel, OK);
			}
			else
			{
				enviar_respuesta(socket_kernel, ERROR);
			}
			break;

		case LEER_ARCHIVO:
			break;
		
		case TRUNCAR_ARCHIVO:
			nombre_archivo = recibir_mensaje(socket_kernel);
			recv(socket_kernel, &tam_archivo, sizeof(uint32_t), MSG_WAITALL);
			printf("Truncar %s a %u\n", nombre_archivo, tam_archivo);
			truncar_archivo(nombre_archivo, tam_archivo);
			break;

		case RESET_FILE_SYSTEM: 
			//creacionFilesystem(filesystem,path_bloques);
			break;

		case RESET_FAT: 
			//reiniciar_fat(fat,ui32_max_entradas_fat);
			break;

		case MOSTRAR_TABLA_FAT: 
			mem_hexdump(fat->memory_map, (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t));
			break;

		case FIN_DE_PROGRAMA: 
			//fclose(fat);
			//fclose(filesystem);
			liberar_conexion(socket_kernel);
			return EXIT_SUCCESS;
			break;

		default:
			liberar_conexion(socket_kernel);
			return;
			break;
		}
	}
	return 0;
	}