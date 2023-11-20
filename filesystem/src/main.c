#include "main.h"


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
	tam_archivo = fcb->tam_archivo;
	liberar_fcb(fcb);
	printf("aa_tam_archivo = %i\n", tam_archivo);
    return tam_archivo;
}

uint32_t crear_archivo(char* path_fcb, char* nombre)
{
    t_fcb* fcb = malloc(sizeof(t_fcb));
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
	config_set_value(config_fcb, "NOMBRE_ARCHIVO", nombre);
	config_set_value(config_fcb, "TAMANIO_ARCHIVO", "0");
	config_set_value(config_fcb, "BLOQUE_INICIAL", "0");
	config_save(config_fcb);
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    close(archivo_fcb);
    return 1;
}

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
	char* cant_bloques_total=config_get_string_value(config,"CANT_BLOQUES_TOTAL");
    char* cant_bloques_swap=config_get_string_value(config,"CANT_BLOQUES_SWAP");
    char* tam_bloque=config_get_string_value(config,"TAM_BLOQUE");
    //char* retardo_acceso_bloque=config_get_string_value(config,"RETARDO_ACCESO_BLOQUE");
    //char* retardo_acceso_fat=config_get_string_value(config,"RETARDO_ACCESO_FAT");

    ui32_cant_bloques_total=atoi(cant_bloques_total);
    ui32_cant_bloques_swap=atoi(cant_bloques_swap);
    ui32_max_entradas_fat=ui32_cant_bloques_total-ui32_cant_bloques_swap;
    ui32_tam_bloque=atoi(tam_bloque);

    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha);

    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);

    int socket_servidor = iniciar_servidor(logger, puerto_escucha);
    int socket_kernel = esperar_cliente(logger, socket_servidor);
    if(socket_kernel){
        log_info(logger,"Se conectó kernel");
    }

	//Borra los archivos fcb para realizar las pruebas
	/*
	printf("----------------------------------------------------------\n");
	printf(">>Se borran los archivos .fcb creados previamente\n");
	printf("----------------------------------------------------------\n");
	remove("../filesystem/fcbs/documento1.fcb");
	remove("../filesystem/fcbs/documento2.fcb");
	remove("../filesystem/fcbs/documento3.fcb");
    //Creación e inicialización de Filesystem y Fat
	*/
	/*-------------------------------------------------------*/
	/*
	printf("----------------------------------------------------------\n");
    filesystem=iniciarArchivoFilesystem(filesystem,path_bloques);
	printf ("---> Apertura filesystem: bloques.dat <ok>\n");
    filesystem=creacionFilesystem(filesystem,path_bloques); //(QUIRAR A FUTUTO)reset temporal de archivo bloques.dat
	printf ("---> Reset filesystem: bloques.dat <ok>\n");
	*/
	/*-------------------------------------------------------*/
	/*
	fat=iniciarArchivoFAT(fat,path_fat,ui32_max_entradas_fat);
	printf ("---> Apertura de archivo fat.dat: <ok>\n");
	fat=reiniciar_fat(fat,ui32_max_entradas_fat); //(QUIRAR A FUTUTO)Reset de fat.dat para probar la creación de la tabla
	printf ("---> Reset tabla FAT <ok>\n");
	*/
	/*-------------------------------------------------------*/
	/*
	//Documento para probar el funcionamiento del Filesystem y Fat
	printf ("---> Emulación de contenido en memoria de: documento1.txt <ok>\n");
    ui32_tam_de_archivo=abrirDocumento("../filesystem/archivo_datos/documento1.txt",documentoArchivo);
	printf("----------------------------------------------------------\n");
	*/
    op_code operacion;
	char* nombre_archivo;
	int32_t tam_archivo;
	while(1)
	{
		operacion = recibir_operacion(socket_kernel);
		printf ("\nSe recibe operacion:%d\n",operacion);
		switch (operacion)
		{
		case ABRIR_ARCHIVO:
			nombre_archivo = recibir_mensaje(socket_kernel);
			tam_archivo = abrir_archivo(path_fcb, nombre_archivo);
			printf("m_tam_archivo = %i\n", tam_archivo);
			send(socket_kernel, &tam_archivo, sizeof(int32_t), NULL);
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
		case RESET_FILE_SYSTEM: 
			creacionFilesystem(filesystem,path_bloques);
			break;
		case RESET_FAT: 
			reiniciar_fat(fat,ui32_max_entradas_fat);
			break;
		case MOSTRAR_TABLA_FAT: 
			mostrar_tabla_FAT(fat,ui32_max_entradas_fat);
			break;
		case FIN_DE_PROGRAMA: 
			fclose(fat);
			fclose(filesystem);
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