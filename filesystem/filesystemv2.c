#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <readline/readline.h>
#include "utils_filesystem.h"
#include "utils_bloques.h"

/*-------------------------------------------*/
#define PATH_FAT "fat.dat"
#define PATH_BLOQUES "bloques.dat"
#define PATH_FCB "/fcbs"
/*-------------------------------------------*/
#define CANT_BLOQUES_TOTAL 1024
#define CANT_BLOQUES_SWAP 64
#define TAM_BLOQUE 1024
//#define UINT32_MAX
/*-------------------------------------------*/
FILE *filesystem;
FILE *fcb;
FILE *fat;

char documentoArchivo[30000]="";
uint32_t MAX_ENTRADAS_FAT=CANT_BLOQUES_TOTAL-CANT_BLOQUES_SWAP;

/*---------------------------------------------------------------------------------*/
/*-- Creacion de Filesystem - archivo: bloques.dat --*/

int existeArchivoFilesystem() {
    filesystem=fopen (PATH_BLOQUES,"rb");
    if (filesystem==NULL) return (0); /*Retorna que el archivo NO existe */
    return (1); /*Retorna que el archivo existe */
}

int creacionFilesystem() {
    uint32_t unBloque32Bits=0;
    filesystem=fopen (PATH_BLOQUES,"w+b");
    if (filesystem==NULL) {
        printf("No fue posible crear el archivo bloques.dat\n");
        return(0);
    }
    else {
        for (int i=0;i<262143;i++) fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,filesystem);
        return (1);
    }
}

int abrirFilesystem() {
    filesystem=fopen (PATH_BLOQUES,"r+b");
    if (filesystem==NULL) return (0);
    else return (1);
}

int iniciarArchivoFilesystem() {
    if (existeArchivoFilesystem()) {
        if (abrirFilesystem()) {
            printf("filesystem abierto\n");
        }
        else {
            printf("No se pudo abrir el archivo de Filesystem\n");
            return (0);
        }
    }
    else {
        if(creacionFilesystem()) {
            printf("Archivo de Filesystem creado\n");
            return (1);
        }
        else {
            printf("No se pudo crear el archivo de Filesystem\n");
            return (0);
        };
    }
    return (1);
}

/*---------------------------------------------------------------------------------*/
/*-- Creacion de Filesystem - archivo: fat.dat --*/

int existeArchivoFAT() {
    fat=fopen (PATH_FAT,"rb");
    if (fat==NULL) return (0); /*Retorna que el archivo NO existe */
    return (1); /*Retorna que el archivo existe */
}

int creacionFAT() {
    uint32_t unBloque32Bits=0;
    fat=fopen (PATH_FAT,"w+b");
    if (fat==NULL) {
        printf("No fue posible crear el archivo fat.dat\n");
        return(0);
    }
    else {
        int tamanoFAT = (CANT_BLOQUES_TOTAL-CANT_BLOQUES_SWAP);
        for (int i=0;i<tamanoFAT;i++) fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,fat);
        return (1);
    }
}

int abrirFAT() {
    fat=fopen (PATH_FAT,"r+b");
    if (fat==NULL) return (0);
    else return (1);
}

int iniciarArchivoFAT() {
    if (existeArchivoFAT()) {
        if (abrirFAT()) {
            printf("Archivo tabla FAT abierto\n");
        }
        else {
            printf("No se pudo abrir el archivo tabla FAT\n");
            return (0);
        }
    }
    else {
        if(creacionFAT()) {
            printf("Archivo de tabla FAT creado\n");
            return (1);
        }
        else {
            printf("No se pudo crear el archivo de tabla FAT\n");
            return (0);
        };
    }
    return (1);
}


int abrirDocumento(char *nombreArchivo) {
    FILE *f;
    int i=0;
    char caracter;
    long tamArchivo = tamanioArchivo(nombreArchivo);
    f=fopen(nombreArchivo,"r");
    for (i=0;i<tamArchivo;i++) {
        fread(&caracter,sizeof(caracter),1,f);
        documentoArchivo[i]=caracter;
    }
    documentoArchivo[tamArchivo]='\0';
    fclose(f);
    return(tamArchivo);
}
/*---------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/* Emulacion de peticiones del modulo Kernel
/*-----------------------------------------------------------------------------------*/
/* 1) ABRIR_ARCHIVO-----------------------------*/
int abrir_archivo(char *nombreArchivo) {



}

/* 2) CREAR_ARCHIVO-----------------------------*/
int crear_archivo(char *nombreArchivo) {
    /*Crear el archivo .fcb con los parametros incializados*/
    actualizar_Archivo_fcb(nombreArchivo,0,9999);
    return(1);
}
/* 3) TRUNCAR_ARCHIVO-----------------------------*/
int truncar_archivo(char *nombreArchivo,uint32_t ui32_longMen_datos) {
    FILE *arch_fcb;
    uint32_t ui32_entrada_inicial;
    uint32_t ui32_cant_bloques_necesitados;
    uint32_t ui32_cant_bloques_libres;


    printf("OP:TRUNCAR_ARCHIVO\n");
    ui32_entrada_inicial=bloqueInicial_Archivo_fcb(nombreArchivo);
    if (ui32_entrada_inicial==9999) {
        /*El archivo fcb NO tiene una entrada inicial asignada en la tabla FAT*/
        ui32_cant_bloques_necesitados=cantBloques_FAT_necesitados(ui32_longMen_datos,TAM_BLOQUE);
        ui32_cant_bloques_libres=cantidadBloques_FAT_libres(fat,MAX_ENTRADAS_FAT);
        /*Evalua si la cantidad de entradas en la Tabla FAT son suficientes*/
        if(ui32_cant_bloques_necesitados<=ui32_cant_bloques_libres) {
            printf("Se asignaran las entradas en la tabla FAT\n");
            ui32_entrada_inicial=asignarBloquesFAT(fat,ui32_cant_bloques_necesitados,MAX_ENTRADAS_FAT);
            printf("La entrada_ui32_entrada_inicial es:%u\n",ui32_entrada_inicial);
            actualizar_Archivo_fcb(nombreArchivo,ui32_longMen_datos,ui32_entrada_inicial);
        }
        else {
            printf ("No hay espacio sufienciente en Disco para guardar el archivo\n");
        }
    }
    else {
        /*El archivo fcb SI tiene una entrada inicial asignada en la tabla FAT*/

    }
  
}


/* 4) LEER_ARCHIVO-----------------------------*/
int leer_archivo(char *nombreArchivo) {
    uint32_t ui32_cantBloques_a_leer=0;
    uint32_t ui32_entrada_FAT;
    uint32_t ui32_numero_bloque=0;
    uint32_t indice=0;
    char buffer_lectura[TAM_BLOQUE+1]="";
    char buffer_documento[TAM_BLOQUE*30]=""; //Tamanio del buffer es un punto a analizar porque no esta determinado

    printf("OP:LEER_ARCHIVO\n");
    /*Calcula la cantidad de bloques a escribir en el archivo bloques.dat*/
    /*Obtiene la direccion de la entrada inicial de la tabla FAT*/
    ui32_cantBloques_a_leer=cantBloques_FAT_necesitados(tamanio_Archivo_fcb(nombreArchivo),TAM_BLOQUE);
    ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo);
    for (indice=0;indice<ui32_cantBloques_a_leer;indice++) {
        strcat(buffer_documento,lectura_de_archivo_bloques(filesystem,ui32_entrada_FAT,buffer_lectura,TAM_BLOQUE));
        ui32_entrada_FAT=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
    }
    printf("---------------------------------------------------------------------------------\n");
    printf("El documento leido desde el arhivo: %s es:...\n\n",nombreArchivo);
    printf("%s\n",buffer_documento);
    printf("---------------------------------------------------------------------------------\n");
    printf ("El tamanio del archivo es:%ld bytes\n",strlen(buffer_documento));
    printf("---------------------------------------------------------------------------------\n");

}

/* 5) ESCRIBIR_ARCHIVO-----------------------------*/
int escribir_archivo(char *nombreArchivo,char *documentoArchivo) {
    uint32_t ui32_cantBloques_a_escribir=0;
    uint32_t ui32_entrada_FAT=0;
    uint32_t ui32_numero_bloque=0;
    char buffer_escritura[TAM_BLOQUE];
    uint32_t indice=0;
    
    printf("OP:ESCRIBIR_ARCHIVO\n");
    /*Calcula la cantidad de bloques a escribir en el archivo bloques.dat*/
    /*Obtiene la direccion de la entrada inicial de la tabla FAT*/
    ui32_cantBloques_a_escribir=cantBloques_FAT_necesitados(tamanio_Archivo_fcb(nombreArchivo),TAM_BLOQUE);
    ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo);
    for (indice=0;indice<ui32_cantBloques_a_escribir;indice++) {
        strcpy(buffer_escritura,cargar_buffer_escritura(buffer_escritura,ui32_numero_bloque,documentoArchivo,TAM_BLOQUE));
        escribir_en_archivo_bloques(filesystem,ui32_entrada_FAT,buffer_escritura,TAM_BLOQUE);
        ui32_entrada_FAT=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
        ui32_numero_bloque++;
    }
}

/*---------------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
    uint32_t unBloque32Bits=0;
    uint32_t ui32_tam_de_archivo=0;

    iniciarArchivoFilesystem();
    iniciarArchivoFAT();
    reiniciar_fat(fat,MAX_ENTRADAS_FAT);

    creacionFilesystem(); //reset temporal de archivo bloques.dat
    
    printf ("---> El sistema esta limitado para trabajar con archivos de hasta 40.000 bytes <----")


    printf("-----------------------------------------------------------------------\n");
    ui32_tam_de_archivo=abrirDocumento("documento1.txt");
    printf("El tamanio del archivo de texto es:%u bytes\n",ui32_tam_de_archivo);
    crear_archivo("documento1");
    truncar_archivo("documento1",ui32_tam_de_archivo);
    escribir_archivo("documento1",documentoArchivo);
    leer_archivo("documento1");
    
    printf("-----------------------------------------------------------------------\n");
    ui32_tam_de_archivo=abrirDocumento("documento2.txt");
    printf("El tamanio del archivo de texto es:%u bytes\n",ui32_tam_de_archivo);
    crear_archivo("documento2");
    truncar_archivo("documento2",ui32_tam_de_archivo);
    escribir_archivo("documento2",documentoArchivo);
    leer_archivo("documento2");
    
    printf("-----------------------------------------------------------------------\n");
    ui32_tam_de_archivo=abrirDocumento("documento3.txt");
    printf("El tamanio del archivo de texto es:%u bytes\n",ui32_tam_de_archivo);
    crear_archivo("documento3");
    truncar_archivo("documento3",ui32_tam_de_archivo);
    escribir_archivo("documento3",documentoArchivo);
    leer_archivo("documento3");
    
    printf("-----------------------------------------------------------------------\n");
    ui32_tam_de_archivo=abrirDocumento("documento4.txt");
    printf("El tamanio del archivo de texto es:%u bytes\n",ui32_tam_de_archivo);
    crear_archivo("documento4");
    truncar_archivo("documento4",ui32_tam_de_archivo);
    escribir_archivo("documento4",documentoArchivo);
    leer_archivo("documento4");

    mostrar_tabla_FAT(fat,MAX_ENTRADAS_FAT);
    fclose(fat);
    fclose(filesystem);
    
    return(1);
}