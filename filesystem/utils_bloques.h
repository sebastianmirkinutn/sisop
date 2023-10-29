#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/*
int creacionFilesystem(FILE *filesystem,char *nombreArchivo_filesystem) {
    uint32_t unBloque32Bits=0;
    filesystem=fopen (nombreArchivo_filesystem,"w+b");
    if (filesystem==NULL) {
        printf("No fue posible crear el archivo bloques.dat\n");
        return(0);
    }
    else {
        for (int i=0;i<262143;i++) fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,filesystem);
        return (1);
    }
}
*/

char *lectura_de_archivo_bloques(FILE *filesystem,uint32_t ui32_entrada_FAT,char *buffer_lectura,uint32_t tam_bloque) {
    uint32_t ui32_bloque_lectura=0;

    ui32_bloque_lectura=(64*tam_bloque)+(ui32_entrada_FAT*1024);
    fseek(filesystem,ui32_bloque_lectura,SEEK_SET);
    fread(buffer_lectura,tam_bloque,1,filesystem);
    return(buffer_lectura);
}


int escribir_en_archivo_bloques(FILE *filesystem,uint32_t ui32_entrada_FAT,char *buffer_escritura,uint32_t tam_bloque) {
    uint32_t ui32_bloque_escritura=0;

    ui32_bloque_escritura=(64*tam_bloque)+(ui32_entrada_FAT*1024); /*Cada entrada a la FAT tiene 4 bytes*/
    fseek(filesystem,ui32_bloque_escritura,SEEK_SET);
    fwrite(buffer_escritura,strlen(buffer_escritura),1,filesystem);
    return(1);
}

char *cargar_buffer_escritura(char *buffer_escritura,uint32_t numero_bloque,char *documentoArchivo,uint32_t tam_bloque) {
    uint32_t offset_Bloque=0;
    uint32_t indice_buffer=0;

    offset_Bloque=numero_bloque*tam_bloque;
    while ((documentoArchivo[offset_Bloque]!='\0') && (indice_buffer<tam_bloque)) {
        buffer_escritura[indice_buffer]=documentoArchivo[offset_Bloque];
        indice_buffer++;
        offset_Bloque++;
    }
    buffer_escritura[indice_buffer]='\0';
    return(buffer_escritura);
}

