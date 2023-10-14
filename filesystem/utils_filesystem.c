#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LONG_BUFFER 50


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
uint32_t cantBloques_FAT_Necesitados(uint32_t ui32_longMen_datos,uint32_t tamanioBloque) {
    uint32_t ui32_CantBloques=0;

    ui32_CantBloques=ui32_longMen_datos/tamanioBloque;
    if ((ui32_longMen_datos % tamanioBloque)!=0) ui32_CantBloques++;
    return(ui32_CantBloques);
}

/*Calcula la cantidad de bloques FAT libres disponibles*/
uint32_t cantidadBloques_FAT_libres(FILE *fat,uint32_t ui32_cantBloqueTotales_FAT,uint32_t ui32_cant_bloques_swap) {
    rewind(fat);
    uint32_t ui32_cantBloques_FAT_total=0;
    uint32_t ui32_contBloques_FAT_libres=0;
    uint32_t ui32_indice=0;
    uint32_t data_tabla_FAT=0;

    ui32_cantBloques_FAT_total=ui32_cantBloqueTotales_FAT-ui32_cant_bloques_swap;
    for (ui32_indice=0;ui32_indice<ui32_cantBloques_FAT_total;ui32_indice++) {
        fread (&data_tabla_FAT,sizeof(data_tabla_FAT),1,fat);
        if (data_tabla_FAT==0) ui32_contBloques_FAT_libres++;
    }
    rewind(fat);
    return (ui32_contBloques_FAT_libres);
}

/*Busca un bloque libre en la tabla FAT*/
uint32_t buscarBloqueFatDisponible(FILE *fat,uint32_t ui32_cantBloqueTotales_FAT,uint32_t ui32_cant_bloques_swap) {
    uint32_t ui32_data_bloque_tabla_FAT;
    uint32_t ui32_Bloque=0;
    uint32_t ui32_indiceBloque=0;
    uint32_t ui32_cantBloques_FAT_total;

    ui32_indiceBloque=0;
    ui32_cantBloques_FAT_total=ui32_cantBloqueTotales_FAT-ui32_cant_bloques_swap;
    rewind(fat);
    fread (&ui32_data_bloque_tabla_FAT,sizeof(ui32_data_bloque_tabla_FAT),1,fat);
    while ((ui32_data_bloque_tabla_FAT!=0) && (ui32_indiceBloque<ui32_cantBloques_FAT_total)) {
        ui32_indiceBloque++;
        fread (&ui32_data_bloque_tabla_FAT,sizeof(ui32_data_bloque_tabla_FAT),1,fat);
    }
    fseek(fat,-4,SEEK_CUR);
    return (ui32_indiceBloque*4);
}

/*Asigna bloques de la tabla FAT*/
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

/*Muestrar el contenido del archivo de la tabla FAT - Limitado a primeros 40 bloques*/
void mostrar_tabla_FAT(FILE *fat,uint32_t ui32_cantBloqueTotales_FAT,uint32_t ui32_cant_bloques_swap) {
    uint32_t ui32_bloqueFAT=0;
    uint32_t ui32_cantBloques_FAT_total=0;
    uint32_t ui32_indice=0;

    ui32_cantBloques_FAT_total=ui32_cantBloqueTotales_FAT-ui32_cant_bloques_swap;
    rewind(fat);
    printf ("---------------------------------------\n");
    printf ("---------Contenido de la FAT-----------\n");
    printf ("---------------------------------------\n");
    for (ui32_indice=0;ui32_indice<40;ui32_indice++) { 
        fread (&ui32_bloqueFAT,sizeof(ui32_bloqueFAT),1,fat);
        if (ui32_bloqueFAT!=9999) printf ("Bloque:%u tiene el dato:%u\n",ui32_indice,ui32_bloqueFAT/4);
        else printf ("Bloque:%u tiene el dato:%u\n",ui32_indice,ui32_bloqueFAT);
    }
    printf ("---------------------------------------\n");
    rewind(fat);
}

int main() {
    printf("El tamanio del archivo es:%u\n",tamanio_Archivo_fcb("prueba1"));
    printf("El bloque inicial es:%u\n",bloqueInicial_Archivo_fcb("prueba1"));
    printf ("Cantidad de bloques necesarios:%u\n",cantBloques_FAT_Necesitados(13308,1024));
    FILE *fat;
    fat=fopen ("fat.dat","rb");
    printf ("Cantidad de bloques FAT libres:%u\n",cantidadBloques_FAT_libres(fat,1024,64));
    rewind(fat);
    printf ("El primer bloque FAT libre es:%u\n",buscarBloqueFatDisponible(fat,1024,64));
    mostrar_tabla_FAT(fat,1024,64);
    fclose(fat);
}
