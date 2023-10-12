#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <readline/readline.h>

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

struct regFCB {
    char nombreArchivo[20];
    uint32_t tamanoArchivo;
    uint32_t bloqueInicial;
};


FILE *filesystem;
FILE *fcb;
FILE *fat;

struct regFCB tablaFCB[100];


char documentoArchivo[18432]="";



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
/*-- Creacion de Filesystem - archivo: de tabla FCB --*/



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

/*---------------------------------------------------------------------------------*/
/*-- Asignacion a FAT de un archivo --*/

uint32_t tamanoArchivo (char *nombreArchivo) {
    FILE *f;
    long tamArchivo;
    f=fopen(nombreArchivo,"r");
    fseek(f, 0L, SEEK_END);
    tamArchivo = ftell(f);
    fclose(f);
    return(tamArchivo );
}

uint32_t buscarBloqueFatDisponible() {
    rewind(fat);
    uint32_t bloqueFAT;
    uint32_t iBloque=0;
    uint32_t cantBloques=0;

    iBloque=0;
    cantBloques=CANT_BLOQUES_TOTAL-CANT_BLOQUES_SWAP;
    rewind(fat);
    fread (&bloqueFAT,sizeof(bloqueFAT),1,fat);
    while ((bloqueFAT!=0) && (iBloque<=cantBloques)) {
        iBloque=iBloque+1;
        fread (&bloqueFAT,sizeof(bloqueFAT),1,fat);
    }
    fseek(fat,-4,SEEK_CUR);
    return (iBloque*4);
}


void mostrarContenidoArchivoFAT() {
    uint32_t bloqueFAT;
    rewind(fat);
    printf ("---------------------------------------\n");
    printf ("---------Contenido de la FAT-----------\n");
    printf ("---------------------------------------\n");
    for (int i=0;i<=30;i++) { 
        fread (&bloqueFAT,sizeof(bloqueFAT),1,fat);
        if (bloqueFAT!=9999) printf ("Bloque:%d tiene el dato:%u\n",i,bloqueFAT/4);
        else printf ("Bloque:%d tiene el dato:%u\n",i,bloqueFAT);
    }
    printf ("---------------------------------------\n");
    rewind(fat);
}


int asignarBloquesFAT(char *nombreArchivo) {
    uint32_t index_bloqueFAT=0;
    uint32_t index_bloqueFAT_anterior=0;
    uint32_t data_bloqueFAT=0;
    uint32_t i;
    uint32_t tamArchivo;

    /* ------- Completar datos de la FCB ----- */
    strcpy(tablaFCB[0].nombreArchivo,nombreArchivo);
    tablaFCB[0].tamanoArchivo = tamanoArchivo(nombreArchivo);
    index_bloqueFAT=buscarBloqueFatDisponible();
    tablaFCB[0].bloqueInicial = index_bloqueFAT;
    /*-----------------------------------------*/
    /* Asigna el primer bloque disponible */
    data_bloqueFAT=9999;
    fwrite (&data_bloqueFAT,sizeof(data_bloqueFAT),1,fat);
    /*------------------------------------------*/
    tamArchivo=(tablaFCB[0].tamanoArchivo)/1024;
    printf("La cantidad de bloques necesitados por el archivo es:%d\n",tamArchivo);
    index_bloqueFAT_anterior=ftell(fat)-4;
    for (i=0;i<tamArchivo;i++) {
        /*----------------------------------------------*/
        index_bloqueFAT=buscarBloqueFatDisponible();
        data_bloqueFAT=9999;
        fwrite (&data_bloqueFAT,sizeof(data_bloqueFAT),1,fat);
        /*----------------------------------------------*/
        fseek(fat,index_bloqueFAT_anterior,SEEK_SET);
        data_bloqueFAT=index_bloqueFAT;
        fwrite (&data_bloqueFAT,sizeof(data_bloqueFAT),1,fat);
        /*----------------------------------------------*/
        index_bloqueFAT_anterior=index_bloqueFAT;
        index_bloqueFAT=index_bloqueFAT+4;
        fseek(fat,index_bloqueFAT,SEEK_CUR);
    }
    return (1);
}

int reiniciar_fat() {
    uint32_t unBloque32Bits=0;
    int tamanoFAT = (CANT_BLOQUES_TOTAL-CANT_BLOQUES_SWAP);
    for (int i=0;i<tamanoFAT;i++) fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,fat);
    rewind(fat);
}

int abrirDocumento(char *nombreArchivo) {
    FILE *f;
    int i=0;
    char caracter;
    long tamArchivo = tamanoArchivo(nombreArchivo);
    f=fopen(nombreArchivo,"r");
    for (i=0;i<tamArchivo;i++) {
        fread(&caracter,sizeof(caracter),1,f);
        documentoArchivo[i]=caracter;
    }
    documentoArchivo[tamArchivo]='\0';
    printf ("%s\n",documentoArchivo);
    fclose(f);
    return(1);
}
/*---------------------------------------------------------------------------------*//
int crear_archivo(char *nombreArchivo) {
    char nombreArchivo[100]="./fcbs/";
    strcat(nombreArchivo,argv[2]);
    strcat(nombreArchivo,".fcb");
    printf ("%s\n",nombreArchivo);
    f=fopen((nombreArchivo),"w");
    char datos[100]="NOMBRE_ARCHIVO=";
    strcat(datos,argv[2]);
    fwrite(datos,sizeof(datos),1,f);
    strcpy(datos,"TAMANIO_ARCHIVO=0");
    fwrite(datos,sizeof(datos),1,f);
    strcpy(datos,"BLOQUE_INICIAL=9999");
    fwrite(datos,sizeof(datos),1,f);
    fclose(f);
}
/*---------------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
    uint32_t unBloque32Bits=0;
    int parametrosCorrectos=0;
    FILE *f;

    iniciarArchivoFilesystem();
    iniciarArchivoFAT();

    if (argc==2) {
        if (!(strcmp(argv[1],"reiniciar_fat"))) {
            reiniciar_fat();
            parametrosCorrectos=1;
        }
        if (!(strcmp(argv[1],"mostrar_fat"))) {
            mostrarContenidoArchivoFAT();
            parametrosCorrectos=1;
        }

    }

    if (argc==3) {
        if (!(strcmp(argv[1],"asignar_fat"))) {
            abrirDocumento(argv[2]);
            asignarBloquesFAT(argv[2]);
            parametrosCorrectos=1;
        }

        if (!(strcmp(argv[1],"liberar_bloque"))) {
            rewind(fat);
            fseek(fat,atol(argv[2])*4,SEEK_SET);
            fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,fat);
            rewind(fat);
            printf("Bloque restablecido a 0 en tabla FAT\n");
            parametrosCorrectos=1;
        }

        if (!(strcmp(argv[1],"crear_archivo"))) {
            crear_archivo(argv[2]);
            parametrosCorrectos=1;
        }
    }
    if (!(parametrosCorrectos)) {
        printf("\n----- listado de parametros --------\n");
        printf("Reset de archivo FAT: ./filesystemv2.o reiniciar_fat\n\n");
        printf("Mostrar contenido archivo FAT: ./filesystemv2.o mostrar_fat\n\n");
        printf("Asignar FAT a un archivo: ./filesystemv2.o asignar_fat nombreArchivo.fcb\n\n");
        printf("Libera un bloque de la FAT: ./filesystemv2.o liberar_bloque numBloque\n\n");
        printf("Crear un archivo nuevo: ./filesystemv2.o crear_archivo nombreArchivo\n\n");
    }

    fclose(fat);
    fclose(filesystem);
    return(1);
}
