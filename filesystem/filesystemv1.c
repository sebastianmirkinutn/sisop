#include <string.h>
#include <stdint.h>
#include <commons/log.h>
#include <commons/config.h>
#include <readline/readline.h>

/*-------------------------------------------*/
#define PATH_FAT "fat.dat"
#define PATH_BLOQUES "bloques.dat"
#define PATH_FCB "fcbs"
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

    iBloque=0;
    fread (&bloqueFAT,sizeof(bloqueFAT),1,fat);
    printf ("Contenido de bloque:%u\n",bloqueFAT);
    while ((bloqueFAT!=0) && (iBloque<=20)) {
        iBloque++;
        fread (&bloqueFAT,sizeof(bloqueFAT),1,fat);
        printf ("Contenido de bloque:%u\n",bloqueFAT);
    }
    printf ("El bloque libre disponible es:%d\n",iBloque);
    return (iBloque);
}

int asignarBloquesFAT(char *nombreArchivo) {
    uint32_t bloqueFAT;
    uint32_t bloqueFAT_Anterior;
    uint32_t tamArchivo;
    int cantBloquesNecesitados;
    int i=0;
    //--------- En proceso de desarrollo
    tamArchivo=tamanoArchivo(nombreArchivo)
    cantBloquesNecesitados=tamArchivo/TAM_BLOQUE;
    strcpy(tablaFCB[0].nombreArchivo,nombreArchivo);
    tablaFCB[0].tamanoArchivo=tamArchivo;
    bloqueFAT=buscarBloqueFatDisponible();
    tablaFCB[0].bloqueInicial = bloqueFAT;
    fseek(fat,-1,SEEK_CUR);
    fwrite (&bloqueFAT,sizeof(bloqueFAT),1,fat);
    bloqueFAT_Anterior=ftell(fat)-1;

    for (i=0;i<cantBloquesNecesitados-1;i++) {
        bloqueFAT=buscarBloqueFatDisponible();
        fseek(fat,-1,SEEK_CUR);
        fwrite (&bloqueFAT,sizeof(bloqueFAT),1,fat);
    }

    return (1);
}

void recorrerArchivoFAT() {
    uint32_t bloqueFAT;
    rewind(fat);
    printf ("---------------------------------------\n");
    printf ("---------Contenido de la FAT-----------\n");
    printf ("---------------------------------------\n");
    for (int i=0;i<=20;i++) { 
        fread (&bloqueFAT,sizeof(bloqueFAT),1,fat);
        printf ("Bloque:%d tiene el dato:%u\n",i,bloqueFAT);
    }
    printf ("---------------------------------------\n");

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

/*---------------------------------------------------------------------------------*/
int main() {
    char nombreArchivo[20]="documento1.fcb";
    abrirDocumento(nombreArchivo);
    if (iniciarArchivoFilesystem()) {
        if (iniciarArchivoFAT()) {
            asignarBloquesFAT(nombreArchivo);
            recorrerArchivoFAT();
            fclose(fat);
        }
        fclose(filesystem);
    }
    return(1);
}
