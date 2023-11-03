#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <commons/log.h>
#include <commons/config.h>
#include <log/log_utils.h>
#include <sockets/sockets.h>
#include <sockets/client_utils.h>
#include <sockets/server_utils.h>
#include <pcb/pcb.h>
#include "utils_operaciones_filesystem.h"
#include "utils_bloques.h"
#include "utils_committed_logger.h"
#include "fcb.h"

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

#endif