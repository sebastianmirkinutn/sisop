#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <readline/readline.h>
#include "utils_adicionales.h"
#include "utils_filesystem.h"
#include "utils_bloques.h"
#include "utils_configuracion.h"
#include "utils_committed_logger.h"
#include "utils.h"
#include "utils_operaciones_filesystem.h"


#define RESET_COLOR    "\x1b[0m"
#define NEGRO_T        "\x1b[30m"
#define NEGRO_F        "\x1b[40m"
#define ROJO_T         "\x1b[31m"
#define ROJO_F         "\x1b[41m"
#define VERDE_T        "\x1b[32m"
#define VERDE_F        "\x1b[42m"
#define AMARILLO_T     "\x1b[33m"
#define AMARILLO_F     "\x1b[43m"
#define AZUL_T         "\x1b[34m"
#define AZUL_F         "\x1b[44m"
#define MAGENTA_T      "\x1b[35m"
#define MAGENTA_F      "\x1b[45m"
#define CYAN_T         "\x1b[36m"
#define CYAN_F         "\x1b[46m"
#define BLANCO_T       "\x1b[37m"
#define BLANCO_F       "\x1b[47m"

/*-------------------------------------------*/
#define CANT_BLOQUES_TOTAL 1024
#define CANT_BLOQUES_SWAP 64
#define TAM_BLOQUE 1024
//#define UINT32_MAX
/*-------------------------------------------*/
int conexion;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha;
char *path_fat;
char *path_bloques;
char *path_fcb;
char *cant_bloques_total;
char *cant_bloques_swap;
char *tam_bloque;
char *retardo_acceso_bloque;
char *retardo_acceso_fat;

 /*--------------------------------------------*/
FILE *filesystem;
FILE *fcb;
FILE *fat;
/*--------------------------------------------*/

char documentoArchivo[30000]="";


void iterator(char* value) {
	//log_info(logger,"%s", value);
	puts(value);
}

/*---------------------------------------------------------------------------------*/
int main(void) {
    uint32_t ui32_tam_de_archivo=0;
    uint32_t ui32_max_entradas_fat;
    uint32_t ui32_cant_bloques_total=0;
	uint32_t ui32_cant_bloques_swap;
	uint32_t ui32_tam_bloque;
    t_log* logger=NULL;
    t_config* config=NULL;
    char *c_direccion_ip_filesystem="127.0.0.1";


    //Creación de archivo auditor logger
    logger=startUpLogger(logger);


    //Lectura de datos de configuración
    config=startUpConfig(config);
    ip_memoria=leerConfiguracion_ip_memoria(config);
    puerto_memoria=leerConfiguracion_puerto_memoria(config);
    puerto_escucha=leerConfiguracion_puerto_escucha(config);
    path_fat=leerConfiguracion_path_fat(config);
    path_bloques=leerConfiguracion_path_bloques(config);
    path_fcb=leerConfiguracion_path_fcb(config);
    cant_bloques_total=leerConfiguracion_cant_bloques_total(config);
    cant_bloques_swap=leerConfiguracion_cant_bloques_swap(config);
    tam_bloque=leerConfiguracion_tam_bloque(config);
    retardo_acceso_bloque=leerConfiguracion_retardo_acceso_bloque(config);
    retardo_acceso_fat=leerConfiguracion_retardo_acceso_fat(config);

    ui32_cant_bloques_total=atoi_(cant_bloques_total);
    ui32_cant_bloques_swap=atoi_(cant_bloques_swap);
    ui32_max_entradas_fat=ui32_cant_bloques_total-ui32_cant_bloques_swap;
    ui32_tam_bloque=atoi_(tam_bloque);

    //Creación e inicialización de Filesystem y Fat
    iniciarArchivoFilesystem(filesystem,path_bloques);
    fat=iniciarArchivoFAT(fat,path_fat,ui32_max_entradas_fat);

    //Configuraciones temporales para realizar pruebas
    fat=reiniciar_fat(fat,ui32_max_entradas_fat); //(QUIRAR A FUTUTO)Reset de fat.dat para probar la creación de la tabla
    creacionFilesystem(filesystem,path_bloques); //(QUIRAR A FUTUTO)reset temporal de archivo bloques.dat
    //Documento para probar el funcionamiento del Filesystem y Fat
    ui32_tam_de_archivo=abrirDocumento("documento1.txt",documentoArchivo);

    //-----------------------------------------------------------------------------------------------------------

	int server_fd = iniciar_servidor(c_direccion_ip_filesystem,puerto_escucha);
	t_list* lista;

	puts("SERVIDOR: Esperando una conexión entrante:");
	int cliente_fd = esperar_cliente(server_fd);
	puts("SERVIDOR: Se detectó una solicitud de conexión:");

	int cod_op=-1;
	int mensaje_recibido=0;
	while (1) {
		cod_op = recibir_operacion(cliente_fd);
		printf("Se recibió el cod_op:%d\n",cod_op);
		switch (cod_op) {
			case MENSAJE:
				mensaje_recibido=recibir_mensaje(cliente_fd);
				printf("-------------------------------------------------------------\n");
				printf("La operacion con archivo recibida es:%d\n",mensaje_recibido);
				switch (mensaje_recibido){
					case 1:
						creacionFilesystem(filesystem,path_bloques);
						break;
					case 2:
						reiniciar_fat(fat,ui32_max_entradas_fat);
						break;
					case 3:
						abrir_archivo("documento1",path_fcb);
						break;
					case 4:
						crear_archivo("documento1",logger,path_fcb);
						break;
					case 5:
						truncar_archivo("documento1",ui32_tam_de_archivo,logger,fat,ui32_tam_bloque,ui32_max_entradas_fat,path_fcb);
						break;
					case 6:
						leer_archivo("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb);
						break;
					case 7:
						escribir_archivo("documento1",documentoArchivo,logger,fat,ui32_tam_bloque,filesystem,path_fcb);
						break;
					case 8:
						mostrar_tabla_FAT(fat,ui32_max_entradas_fat);
						break;
					case 9:
					    fclose(fat);
					    fclose(filesystem);
					    return EXIT_SUCCESS;
						break;
					default:
						puts("Opción incorrecta");
					break;
				}
				break;
			case PAQUETE:
				lista = recibir_paquete(cliente_fd);
				list_iterate(lista, (void*) iterator);
				break;
			case -1:
				return EXIT_FAILURE;
			default:
				puts("La operacion recibida es desconocida");
				break;
			}
	}

}
