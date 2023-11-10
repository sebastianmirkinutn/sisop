#include "main.h"

uint32_t abrir_archivo(char* path_fcb, char* nombre)
{
    t_fcb* fcb = malloc(sizeof(t_fcb));
    char* ruta = path_fcb;
	uint32_t tam_archivo;
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	FILE* archivo_fcb = open(ruta, O_RDONLY);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	fseek(archivo_fcb, 0, SEEK_END);
	tam_archivo = ftell(archivo_fcb);
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    fclose(archivo_fcb);
    return tam_archivo;
}

int main(int argc, char* argv[]) {

	uint32_t ui32_tam_de_archivo=0;
    uint32_t ui32_max_entradas_fat=0;
    uint32_t ui32_cant_bloques_total=0;
	uint32_t ui32_cant_bloques_swap=0;
	uint32_t ui32_tam_bloque=0;

	


	//------------------------------------------------------------------------
    t_log* logger = iniciar_logger("log_filesystem.log","FILESYSTEM");
    t_config* config = iniciar_config("../filesystem/cfg/filesystem.config");
    
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
    //Creación e inicialización de Filesystem y Fat
	/*-------------------------------------------------------*/
	printf("----------------------------------------------------------\n");
    filesystem=iniciarArchivoFilesystem(filesystem,path_bloques);
	printf ("---> Apertura filesystem: bloques.dat <ok>\n");
    filesystem=creacionFilesystem(filesystem,path_bloques); //(QUIRAR A FUTUTO)reset temporal de archivo bloques.dat
	printf ("---> Reset filesystem: bloques.dat <ok>\n");
	/*-------------------------------------------------------*/
	fat=iniciarArchivoFAT(fat,path_fat,ui32_max_entradas_fat);
	printf ("---> Apertura de archivo fat.dat: <ok>\n");
	fat=reiniciar_fat(fat,ui32_max_entradas_fat); //(QUIRAR A FUTUTO)Reset de fat.dat para probar la creación de la tabla
	printf ("---> Reset tabla FAT <ok>\n");
	/*-------------------------------------------------------*/
	//Documento para probar el funcionamiento del Filesystem y Fat
	printf ("---> Emulación de contenido en memoria de: documento1.txt <ok>\n");
    ui32_tam_de_archivo=abrirDocumento("../filesystem/archivo_datos/documento1.txt",documentoArchivo);
	printf("----------------------------------------------------------\n");
    op_code operacion;

    while(1)
    {
		operacion = recibir_operacion(socket_kernel);
		printf ("\nSe recibe operacion:%d\n",operacion);
		if (operacion==0) {
			//char* nombre = recibir_mensaje(socket_kernel);
			//uint32_t tam_archivo = abrir_archivo(nombre, path_fcb);
		}
		else if (operacion==5000){
			creacionFilesystem(filesystem,path_bloques);
		}
		else if (operacion==5001){
			reiniciar_fat(fat,ui32_max_entradas_fat);
		}
		else if (operacion==5002){
			abrir_archivo("../filesystem/archivo_datos/documento1",path_fcb);
		}
		else if (operacion==5003){
			crear_archivo("documento1",logger,path_fcb);
		}
		else if (operacion==5004){
			truncar_archivo("documento1",ui32_tam_de_archivo,logger,fat,ui32_tam_bloque,ui32_max_entradas_fat,path_fcb);
		}
		else if (operacion==5005){
			leer_archivo("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb);
		}
		else if (operacion==5006){
			escribir_archivo("documento1",documentoArchivo,logger,fat,ui32_tam_bloque,filesystem,path_fcb);
		}
		else if (operacion==5007){
			mostrar_tabla_FAT(fat,ui32_max_entradas_fat);
		}
		else if (operacion==5008){
		    fclose(fat);
		    fclose(filesystem);
		    return EXIT_SUCCESS;
		}
		else {
			liberar_conexion(socket_kernel);
			return;
        }
    }
    return 0;
}