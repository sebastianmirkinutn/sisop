#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils_adicionales.h"

#define LONG_BUFFER 50


int reiniciar_fat(FILE *fat,uint32_t MAX_ENTRADAS_FAT) {
    rewind(fat);
    uint32_t unBloque32Bits=0;
    for (int i=0;i<MAX_ENTRADAS_FAT;i++) fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,fat);
    rewind(fat);
    return(1);
}

/* Obtiene el tamanio del archivo*/
uint32_t tamanioArchivo (char *nombreArchivo) {
    FILE *f;
    long tamArchivo;
    f=fopen(nombreArchivo,"r");
    fseek(f, 0L, SEEK_END);
    tamArchivo = ftell(f);
    fclose(f);
    return(tamArchivo );
}


/* Obtiene del archivo fcb el tamanio del archivo del usuario*/
uint32_t tamanio_Archivo_fcb(char *nombreArchivo) {
    FILE *f_arch_fcb;
    char direccionArchivo[100]="./fcbs/";
    char cDatoLeido[LONG_BUFFER];
    char cTamanoArchivo[10]="";
    int i=0;
    int j=0;
    int tope=0;
    int numeroTamanioArchivo;

    strcat(direccionArchivo,nombreArchivo);
    strcat(direccionArchivo,".fcb");
    f_arch_fcb=fopen(direccionArchivo,"r");
    fgets(cDatoLeido,LONG_BUFFER,f_arch_fcb);
    fgets(cDatoLeido,LONG_BUFFER,f_arch_fcb);
    while (cDatoLeido[i++]!='=');
    tope=strlen(cDatoLeido)-i;
    for(j=0;j<tope;j++) cTamanoArchivo[j]=cDatoLeido[i+j];
    cTamanoArchivo[j]='\0';
    numeroTamanioArchivo = atoi(cTamanoArchivo);
    fclose(f_arch_fcb);
    return((uint32_t) numeroTamanioArchivo);
}

/* Obtiene del archivo fcb el bloque de inicio en la tabla FAT del archivo del usuario*/
uint32_t bloqueInicial_Archivo_fcb(char *nombreArchivo) {
    FILE *f_arch_fcb;
    char direccionArchivo[100]="./fcbs/";
    char cDatoLeido[LONG_BUFFER];
    char cBloqueInicio[4]="";
    int i=0;
    int j=0;
    int tope=0;
    int numeroBloqueIncial;

    strcat(direccionArchivo,nombreArchivo);
    strcat(direccionArchivo,".fcb");
    f_arch_fcb=fopen(direccionArchivo,"r");
    fgets(cDatoLeido,LONG_BUFFER,f_arch_fcb);
    fgets(cDatoLeido,LONG_BUFFER,f_arch_fcb);
    fgets(cDatoLeido,LONG_BUFFER,f_arch_fcb);
    while (cDatoLeido[i++]!='=');
    tope=strlen(cDatoLeido)-i;
    for(j=0;j<tope;j++) cBloqueInicio[j]=cDatoLeido[i+j];
    cBloqueInicio[j]='\0';
    numeroBloqueIncial = atoi(cBloqueInicio);
    fclose(f_arch_fcb);
    return((uint32_t) numeroBloqueIncial);
}

/*Calcula la cantidad de bloque FAT necesarios par almacenar datos de memoria*/
/*Retorna la cantidad de bloques necesitados contemplando la fragmentacion interna*/
uint32_t cantBloques_FAT_necesitados(uint32_t ui32_longMen_datos,uint32_t tamanioBloque) {
    uint32_t ui32_CantBloques=0;

    ui32_CantBloques=ui32_longMen_datos/tamanioBloque;
    if ((ui32_longMen_datos % tamanioBloque)!=0) ui32_CantBloques++;
    return(ui32_CantBloques);
}

/*Recorre la tabla FAT contando las entradas libres disponibles*/
/*Retorna la cantidad de entredas a la tabla FAT libres*/
uint32_t cantidadBloques_FAT_libres(FILE *fat,uint32_t MAX_ENTRADAS_FAT) {
    rewind(fat);
    uint32_t ui32_contBloques_FAT_libres=0;
    uint32_t ui32_indice=0;
    uint32_t data_tabla_FAT=0;

    for (ui32_indice=0;ui32_indice<MAX_ENTRADAS_FAT;ui32_indice++) {
        fread (&data_tabla_FAT,sizeof(data_tabla_FAT),1,fat);
        if (data_tabla_FAT==0) ui32_contBloques_FAT_libres++;
    }
    rewind(fat);
    return (ui32_contBloques_FAT_libres);
}

/*Busca un bloque libre en la tabla FAT*/
/*Retorna el primer bloque libre que encuentra*/
uint32_t buscarBloqueFatDisponible(FILE *fat,uint32_t MAX_ENTRADAS_FAT) {
    uint32_t ui32_data_bloque_tabla_FAT;
    uint32_t ui32_indiceBloque=0;

    ui32_indiceBloque=0;
    rewind(fat);
    fread (&ui32_data_bloque_tabla_FAT,sizeof(ui32_data_bloque_tabla_FAT),1,fat);
    while ((ui32_data_bloque_tabla_FAT!=0) && (ui32_indiceBloque<MAX_ENTRADAS_FAT)) {
        ui32_indiceBloque++;
        fread (&ui32_data_bloque_tabla_FAT,sizeof(ui32_data_bloque_tabla_FAT),1,fat);
    }
    fseek(fat,-4,SEEK_CUR);
    return (ui32_indiceBloque*4);
}

/*Asigna bloques de la tabla FAT a un archivo*/
/*Retorna la direccion del bloque inicial de lista enlazada*/
uint32_t asignarBloquesFAT(FILE *fat,uint32_t cant_entradas_FAT_requeridas,uint32_t MAX_ENTRADAS_FAT) {
    uint32_t ui32_entrada_inicial=0;
    uint32_t ui32_entrada_actual=0;
    uint32_t ui32_entrada_anterior=0;
    uint32_t ui32_data_entrada=9999;
    uint32_t i;
    rewind(fat);
    for (i=0;i<cant_entradas_FAT_requeridas;i++) {
        ui32_entrada_actual=buscarBloqueFatDisponible(fat,MAX_ENTRADAS_FAT);
        if (i==0) {
            ui32_entrada_inicial=ui32_entrada_actual;
        }
        else {
            fseek(fat,ui32_entrada_anterior,SEEK_SET);
            ui32_data_entrada=ui32_entrada_actual;
            fwrite (&ui32_data_entrada,sizeof(ui32_data_entrada),1,fat);
            fseek(fat,ui32_entrada_actual,SEEK_SET);
        }
        ui32_data_entrada=9999;
        ui32_entrada_anterior=ui32_entrada_actual;
        fwrite (&ui32_data_entrada,sizeof(ui32_data_entrada),1,fat);
    }
    rewind(fat);
    return (ui32_entrada_inicial/4);
}


int actualizar_Archivo_fcb(char *nombreArchivo,uint32_t ui32_longMen_datos,uint32_t ui32_entrada_inicial) {
    FILE *f_arch_fcb;
    char direccionArchivo[100]="./fcbs/";
    char cInfo_a_Almacenar[100]="";
    char cNumero[10];
    int i=0;
    int j=0;
    int tope=0;
    int numeroBloqueIncial;
    /*------------Apertura de archivo fcb--------------------*/
    strcat(direccionArchivo,nombreArchivo);
    strcat(direccionArchivo,".fcb");
    f_arch_fcb=fopen(direccionArchivo,"w");
    /*-------------------------------------------------------*/
    strcpy(cInfo_a_Almacenar,"NOMBRE_ARCHIVO=");
    strcat(cInfo_a_Almacenar,nombreArchivo);
    strcat(cInfo_a_Almacenar,"\n");
    fwrite(cInfo_a_Almacenar,strlen(cInfo_a_Almacenar),1,f_arch_fcb);

    strcpy(cInfo_a_Almacenar,"TAMANIO_ARCHIVO=");
    strcat(cInfo_a_Almacenar,itoa(ui32_longMen_datos,cNumero));
    strcat(cInfo_a_Almacenar,"\n");
    fwrite(cInfo_a_Almacenar,strlen(cInfo_a_Almacenar),1,f_arch_fcb);

    strcpy(cNumero,"");
    strcpy(cInfo_a_Almacenar,"BLOQUE_INICIAL=");
    strcat(cInfo_a_Almacenar,itoa(ui32_entrada_inicial,cNumero));
    strcat(cInfo_a_Almacenar,"\n");
    fwrite(cInfo_a_Almacenar,strlen(cInfo_a_Almacenar),1,f_arch_fcb);
    /*-------------------------------------------------------*/
    fclose(f_arch_fcb);
    return(1);
}
/*Retorna la siguiente entrada de la tabla FAT siguiendo la lista enlazada*/
uint32_t siguiente_entrada_tabla_FAT(FILE *fat,uint32_t ui32_entrada_FAT) {
    uint32_t ui32_data_bloque_tabla_FAT=0;
    
    rewind(fat);
    ui32_entrada_FAT=ui32_entrada_FAT*4;
    fseek(fat,ui32_entrada_FAT,SEEK_SET);
    fread (&ui32_data_bloque_tabla_FAT,sizeof(ui32_data_bloque_tabla_FAT),1,fat);
    return(ui32_data_bloque_tabla_FAT/4);
}

/*Actualiza una entrada de la tabla FAT con un valor específico*/
int actualizar_entrada_FAT(FILE *fat,uint32_t ui32_entrada,uint32_t ui32_data_entrada) {
    fseek(fat,ui32_entrada*4,SEEK_SET);
    fwrite (&ui32_data_entrada,sizeof(ui32_data_entrada),1,fat);
    return(1);
}


/*Muestrar el contenido del archivo de la tabla FAT - Limitado a primeros 40 bloques*/
void mostrar_tabla_FAT(FILE *fat,uint32_t MAX_ENTRADAS_FAT) {
    uint32_t ui32_bloqueFAT=0;
    uint32_t ui32_indice=0;

    rewind(fat);
    printf ("---------------------------------------\n");
    printf ("---------Contenido de la FAT-----------\n");
    printf ("---------------------------------------\n");
    for (ui32_indice=0;ui32_indice<35;ui32_indice++) { 
        fread (&ui32_bloqueFAT,sizeof(ui32_bloqueFAT),1,fat);
        if (ui32_bloqueFAT!=9999) printf ("Entrada:(%u) se enlaza con:(%u)\n",ui32_indice,ui32_bloqueFAT/4);
        else printf ("Entrada:(%u) Fin de archivo con: (%u)\n",ui32_indice,ui32_bloqueFAT);
    }
    printf ("---------------------------------------\n");
    rewind(fat);
}


/*---------------------------------------------------------------------------*/
/*
int main() {
    printf("El tamanio del archivo es:%u\n",tamanio_Archivo_fcb("prueba1"));
    printf("El bloque inicial es:%u\n",bloqueInicial_Archivo_fcb("prueba1"));
    printf ("Cantidad de bloques necesarios:%u\n",cantBloques_FAT_Necesitados(13308,1024));
    FILE *fat;
    fat=fopen ("fat.dat","r+b");
    reiniciar_fat(fat,960);
    printf ("Cantidad de bloques FAT libres:%u\n",cantidadBloques_FAT_libres(fat,1024,64));
    rewind(fat);
    printf ("El primer bloque FAT libre es:%u\n",buscarBloqueFatDisponible(fat,960));
    rewind(fat);
    asignarBloquesFAT(fat,5,960);
    rewind(fat);
    actualizar_Archivo_fcb("prueba1",13000,5);
    mostrar_tabla_FAT(fat,1024,64);
    fclose(fat);
}*/
}*/
