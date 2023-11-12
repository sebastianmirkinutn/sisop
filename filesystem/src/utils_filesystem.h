#ifndef UTILS_FILESYSTEM_H
#define UTILS_FILESYSTEM_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_adicionales.h"


/*----------------- OPERACIONES RELACIONADAS CON FILESYSTEM -----------------------*/
FILE *existeArchivoFilesystem(FILE *filesystem,char *c_path_bloques);
FILE *creacionFilesystem(FILE *filesystem,char *c_path_bloques);
FILE *abrirFilesystem(FILE *filesystem,char *c_path_bloques);
FILE *iniciarArchivoFilesystem(FILE *filesystem,char *c_path_bloques);
char *obtenerLectura(char *bufferData,char *buffer_bloque,uint32_t posicionPunteroRelativa,uint32_t cantBytes);

/*---------------- OPERACIONES RELACIONADAS CON ARCHIVO FCB -----------------------*/
uint32_t tamanio_Archivo_fcb(char *nombreArchivo,char *c_directorio_fcb);
uint32_t bloqueInicial_Archivo_fcb(char *nombreArchivo,char *c_directorio_fcb);
uint32_t cantBloques_FAT_necesitados(uint32_t ui32_longMen_datos,uint32_t tamanioBloque);
uint32_t cantidadBloques_FAT_libres(FILE *fat,uint32_t MAX_ENTRADAS_FAT);
uint32_t buscarBloqueFatDisponible(FILE *fat,uint32_t MAX_ENTRADAS_FAT);
int actualizar_Archivo_fcb(char *nombreArchivo,uint32_t ui32_longMen_datos,uint32_t ui32_entrada_inicial,char *c_directorio_fcb);

/*--------------------- OPERACIONES RELACIONADAS CON FAT --------------------------*/
int existeArchivoFAT(char *c_path_fat);
FILE *creacionFAT(FILE *fat,char *c_path_fat,uint32_t ui32_tamanio_fat);
FILE *abrirFAT(FILE *fat,char *c_path_fat);
FILE *iniciarArchivoFAT(FILE *fat,char *c_path_fat,uint32_t ui32_tamanio_fat);
FILE *reiniciar_fat(FILE *fat,uint32_t ui32_max_entradas_fat);
uint32_t siguiente_entrada_tabla_FAT(FILE *fat,uint32_t ui32_entrada_FAT);
int actualizar_entrada_FAT(FILE *fat,uint32_t ui32_entrada,uint32_t ui32_data_entrada);
uint32_t asignarBloquesFAT(FILE *fat,uint32_t cant_entradas_FAT_requeridas,uint32_t MAX_ENTRADAS_FAT);
void mostrar_tabla_FAT(FILE *fat,uint32_t MAX_ENTRADAS_FAT);

/*-------------------- OPERACIONES AUXILIARES CON ARCHIVOS ------------------------*/
uint32_t tamanioArchivo (char *nombreArchivo);
uint32_t abrirDocumento(char *nombreArchivo,char *documentoArchivo);
char *solicitarPaginaMemoria(char *paginaDeMemoria);
char *determinaDatosEnPagina(char *paginaDeMemoria,uint32_t ui32_direccionDeMemoria,uint32_t ui32_cantBytes,char *buffer_data);
#endif