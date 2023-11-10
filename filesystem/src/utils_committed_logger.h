#ifndef UTILS_COMMITED_LOGGERS_H
#define UTILS_COMMITED_LOGGERS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/log.h>
#include "utils_adicionales.h"

int committed_logger_CREAR_ARCHIVO(char *nombreArchivo,t_log *logger);
int committed_logger_APERTURA_ARCHIVO(char *nombreArchivo,t_log *logger);
int committed_logger_TRUNCAR(char *nombreArchivo,t_log *logger,char *c_directorio_fcb);
int committed_logger_ESCRITURA_ARCHIVO(char *nombreArchivo,uint32_t ui32_ptrArchivo,uint32_t ui32_direcMemoria,t_log *logger);
int committed_logger_ACCESO_FAT(uint32_t ui32_numEntrada,uint32_t ui32_dataEntrada,t_log *logger);
int committed_logger_ACCESO_BLOQUE_ARCHIVO(char *nombreArchivo,uint32_t ui32_numBloque_Archivo,uint32_t ui32_numBloque_FS,uint32_t ui32_tamBloque,t_log *logger);
int committed_logger_ACCESO_BLOQUE_SWAP(char *nombreArchivo,uint32_t ui32_numBloque_Swap,t_log *logger);


#endif