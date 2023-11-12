#ifndef UTILS_BLOQUES_H
#define UTILS_BLOQUES_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>

char *lectura_de_archivo_bloques(FILE *filesystem,uint32_t ui32_entrada_FAT,char *buffer_lectura,uint32_t tam_bloque);
int escribir_en_archivo_bloques(FILE *filesystem,uint32_t ui32_entrada_FAT,char *buffer_escritura,uint32_t tam_bloque);
char *cargar_buffer_escritura(char *buffer_escritura,uint32_t numero_bloque,char *documentoArchivo,uint32_t tam_bloque);
int escribir_en_archivo_fisico(FILE *filesystem,uint32_t ui32_entrada_FAT,uint32_t ui32_tamBloque,char *buffer_data,uint32_t posicionPunteroRelativa,uint32_t cantBytes);

#endif