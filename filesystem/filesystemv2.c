#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <readline/readline.h>
#include "utils_filesystem.h"
#include "utils_bloques.h"

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
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/* Emulacion de peticiones del modulo Kernel
/*-----------------------------------------------------------------------------------*/
/* 1) ABRIR_ARCHIVO-----------------------------*/
uint32_t abrir_archivo(char *nombreArchivo) {
    FILE *f_arch_fcb;
    char direccionArchivo[100]="./fcbs/";

    strcat(direccionArchivo,nombreArchivo);
    strcat(direccionArchivo,".fcb");
    f_arch_fcb=fopen(direccionArchivo,"r");
    if (f_arch_fcb==NULL) {
        printf("Nombre de archivo incorrecto o no existe");
        exit(1);
    }
    return(tamanio_Archivo_fcb(nombreArchivo));
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
    uint32_t ui32_entrada_FAT;
    uint32_t ui32_entrada_FAT_siguiente;
    uint32_t ui32_cant_bloques_necesitados;
    uint32_t ui32_cant_bloques_asignados;
    uint32_t ui32_cant_bloques_libres;
    uint32_t ui32_cant_bloques_adionales;
    uint32_t ui32_data_entrada;
    uint32_t indice;


    printf("OP:TRUNCAR_ARCHIVO\n");
    ui32_entrada_inicial=bloqueInicial_Archivo_fcb(nombreArchivo);
    
    if (ui32_entrada_inicial==9999) {
        /* Caso (1): Asignacion de entradas a la tabla FAT a un archivo recientemente creado*/ 
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
            /*El archivo fcb NO tiene una entrada inicial asignada en la tabla FAT, pero NO hay mas
            entradas disponibles en la tabla FAT*/
            printf ("No hay espacio sufienciente en Disco para guardar el archivo\n");
        }
    }
    else {
        printf("--- Caso (2) de TRUNCADO, el archivo ya existe -----\n");
        /* Caso (2): El archivo ya tiene entradas en la tabla FAT asignadas*/
        //- Calcula la cantidad de entradas que requeriran los datos de memoria a almacenar
        ui32_cant_bloques_necesitados=cantBloques_FAT_necesitados(ui32_longMen_datos,TAM_BLOQUE);
        //- Calcula la cantidad de entradas que ya tiene el archivo asignados previamente
        ui32_cant_bloques_asignados=cantBloques_FAT_necesitados(tamanio_Archivo_fcb(nombreArchivo),TAM_BLOQUE);
        
        if (ui32_cant_bloques_necesitados<=ui32_cant_bloques_asignados) {
            printf("La cantidad de bloques necesitados por la memoria son:%u\n",ui32_cant_bloques_necesitados);
            printf("La cantidad de ya asignados son:%u\n",ui32_cant_bloques_asignados);
            /*Caso 2A - La cantidad de entradas necesitadas es menor o igual a las que tiene
            ya asignadas el archivo*/
            ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo); //Entrada inicial a la tabla FAT
            
            //Recorre la lista enlazada mientras que cuenta las entradas que necesita la version en memoria del archivo
            for (indice=0;indice<ui32_cant_bloques_necesitados;indice++) {
                if (indice==0) ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo); //Entrada inicial a la tabla FAT
                else ui32_entrada_FAT=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
            }
            printf("La entrada hasta la que necesita la version nueva es:%u\n",ui32_entrada_FAT);
            //Si la ultima entrada que necesita la version actualizada es distinto a 9999 es poque debe liberar entradas
            if ((ui32_entrada_FAT!=9999) && (ui32_entrada_FAT!=2499)){
                ui32_entrada_FAT_siguiente=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
                printf("La siguiente entrada detectada es:%u\n",ui32_entrada_FAT_siguiente);
                ui32_data_entrada=9999;
                actualizar_entrada_FAT(fat,ui32_entrada_FAT,ui32_data_entrada);
                ui32_entrada_FAT=ui32_entrada_FAT_siguiente;
                printf("La entrada actual a actualizar es:%u\n",ui32_entrada_FAT);
                while ((ui32_entrada_FAT!=9999) && (ui32_entrada_FAT!=2499)) {
                    ui32_entrada_FAT_siguiente=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
                    printf("La siguiente entrada detectada es:%u\n",ui32_entrada_FAT_siguiente);
                    ui32_data_entrada=0;
                    actualizar_entrada_FAT(fat,ui32_entrada_FAT,ui32_data_entrada);
                    ui32_entrada_FAT=ui32_entrada_FAT_siguiente;
                }
                ui32_data_entrada=0;
                actualizar_entrada_FAT(fat,ui32_entrada_FAT,ui32_data_entrada);
                
            }
        }
        else {
            /*Caso 2B - La cantidad de entradas necesitadas es mayot a las que tiene
            ya asignadas el archivo almacendo*/
            ui32_cant_bloques_necesitados=cantBloques_FAT_necesitados(ui32_longMen_datos,TAM_BLOQUE);
            ui32_cant_bloques_libres=cantidadBloques_FAT_libres(fat,MAX_ENTRADAS_FAT);
            ui32_cant_bloques_asignados=cantBloques_FAT_necesitados(tamanio_Archivo_fcb(nombreArchivo),TAM_BLOQUE);
            ui32_cant_bloques_adionales=ui32_cant_bloques_necesitados-ui32_cant_bloques_asignados;
            printf("Cantidad de entradas FAT que necesita el documento en memoria son:%u\n",ui32_cant_bloques_necesitados);
            printf("Cantidad de entradas FAT ya asignadas al documento almacenado:%u\n",ui32_cant_bloques_asignados);
            printf("Cantidad de entradas FAT adicionales necesitadas:%u\n",ui32_cant_bloques_adionales);
            printf("Cantidad de entradas FAT libres en la tabla FAT:%u\n",ui32_cant_bloques_libres);
            
            //Recorre la lista de entradas ya asignadas al archivo, en busqueda de la ultima entrada
            ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo); //Entrada inicial a la tabla FAT
            ui32_entrada_FAT_siguiente=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
            //printf("entrada_FAT:%u\n",ui32_entrada_FAT);
            //printf("entrada_FAT_siguiente:%u\n",ui32_entrada_FAT_siguiente);
            while ((ui32_entrada_FAT_siguiente!=9999) && (ui32_entrada_FAT_siguiente!=2499)) {
                ui32_entrada_FAT=ui32_entrada_FAT_siguiente;
                ui32_entrada_FAT_siguiente=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
                //printf("entrada_FAT:%u\n",ui32_entrada_FAT);
                //printf("entrada_FAT_siguiente:%u\n",ui32_entrada_FAT_siguiente);
            }
            ui32_data_entrada=asignarBloquesFAT(fat,ui32_cant_bloques_adionales,MAX_ENTRADAS_FAT)*4;
            actualizar_entrada_FAT(fat,ui32_entrada_FAT,ui32_data_entrada);
        }
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
    //printf("%s\n",buffer_documento);
    printf("---------------------------------------------------------------------------------\n");
    printf ("El tamanio del archivo es:%ld bytes\n",strlen(buffer_documento));
    printf("---------------------------------------------------------------------------------\n");
    return(1);
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
    
    printf ("---> El sistema esta limitado para trabajar con archivos de hasta 40.000 bytes <----");


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
    printf("-----------------------------------------------------------------------\n");
    printf("CASO 1 - Muestra tabla FAT - con las entradas asignadas a 4 archivos\n");
    mostrar_tabla_FAT(fat,MAX_ENTRADAS_FAT);

    printf("---------------------------------------------------------------------------\n\n");
    printf("--- Caso especial para probar la reduccion de entradas en la tabla FAT ----\n\n");
    ui32_tam_de_archivo=abrirDocumento("documento2v2.txt"); /*Variante de documento2.txt con menos byte
    para probar que se liberan entradas FAT cuando se libera espacio*/
    printf("El tamanio del archivo de texto - documento2v2.txt - es:%u bytes\n",ui32_tam_de_archivo);
    //crear_archivo("documento2"); El archivo ya fue creado
    truncar_archivo("documento2",ui32_tam_de_archivo);
    //escribir_archivo("documento2",documentoArchivo);
    //leer_archivo("documento2");
    printf("-----------------------------------------------------------------------\n");
    printf("CASO 2A - Muestra tabla FAT - con 2 entradas liberadas 14 y 15\n");
    printf("...debido a que documeto2.txt fue actualizado borrandole informacion\n");
    mostrar_tabla_FAT(fat,MAX_ENTRADAS_FAT);

    printf("-----------------------------------------------------------------------\n");
    ui32_tam_de_archivo=abrirDocumento("documento5.txt");
    printf("El tamanio del archivo de texto es:%u bytes\n",ui32_tam_de_archivo);
    crear_archivo("documento5");
    truncar_archivo("documento5",ui32_tam_de_archivo);
    escribir_archivo("documento5",documentoArchivo);
    leer_archivo("documento5");
    printf("-----------------------------------------------------------------------\n");
    printf("Muestra tabla FAT - con un nuevo documento documento5.txt - consume 3 entradas\n");
    printf("Usa las dos entradas liberadas 14 y 15, y la 20\n");
    mostrar_tabla_FAT(fat,MAX_ENTRADAS_FAT);
    printf("-----------------------------------------------------------------------\n");

    printf("---------------------------------------------------------------------------\n\n");
    printf("--- Caso especial para probar la ampliacion de entradas en la tabla FAT ----\n\n");
    ui32_tam_de_archivo=abrirDocumento("documento1v2.txt"); /*Variante de documento2.txt con menos byte
    para probar que se liberan entradas FAT cuando se libera espacio*/
    printf("El tamanio del archivo de texto - documento1v2.txt - es:%u bytes\n",ui32_tam_de_archivo);
    //crear_archivo("documento1"); El archivo ya fue creado
    truncar_archivo("documento1",ui32_tam_de_archivo);
    //escribir_archivo("documento1",documentoArchivo);
    //leer_archivo("documento1");
    printf("-----------------------------------------------------------------------\n");
    printf("CASO 2B - Muestra tabla FAT - con la ampliacion de la lista enlazada\n");
    printf("...debido a que documeto1.txt fue actualizado aniadiendo informacion\n");
    mostrar_tabla_FAT(fat,MAX_ENTRADAS_FAT);


    fclose(fat);
    fclose(filesystem);
    
    return(1);
}
