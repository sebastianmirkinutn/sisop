#include "main.h"

uint32_t abrir_archivo(char* path_fcb, char* nombre)
{
    t_fcb* fcb = malloc(sizeof(t_fcb));
    char* ruta = path_fcb;
	uint32_t tam_archivo;
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	FILE* archivo_fcb = open(ruta, O_RDONLY);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	fseek(archivo_fcb, 0, SEEK_END);
	tam_archivo = ftell(archivo_fcb);
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    fclose(archivo_fcb);
    return tam_archivo;
}
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*--------------------- OPERACIONES ADICIONALES DE SOPORTE ------------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
int strlen_(char *texto){
	int i=0;
	while(texto[i++]!='\0');
	return(i-1);
}

char *strcpy_(char *texto1,char *texto2) {
	int i=0;
	while(texto2[i]!='\0') {
		texto1[i]=texto2[i];
		i++;
	}
	texto1[i]='\0';
	return(texto1);
}

uint32_t potencia(uint32_t num1,uint32_t num2) {
    uint32_t resultPotencia=1;
    uint32_t i=0;
    for (i=0;i<num2;i++) {
        resultPotencia=resultPotencia*num1;
    }
    return(resultPotencia);
}

uint32_t atoi_(char *c_cadenaNumero){
	uint32_t ui32_numeroEntero=0;
	uint32_t i_lenCadenaNumero;
	uint32_t i;
    uint32_t i_indice;

	i_lenCadenaNumero=strlen(c_cadenaNumero);
	for (i=0;i<i_lenCadenaNumero;i++) {
        i_indice=i_lenCadenaNumero-i-1;
        ui32_numeroEntero=ui32_numeroEntero+(c_cadenaNumero[i_indice]-48)*potencia(10,i);
	}
	return (ui32_numeroEntero);
}

char *itoa_(uint32_t dividendo,char *cadenaText) {
    uint32_t divisor=10;
    uint32_t resto=0;
    uint32_t cadenaInt[10];
    int i=0;
    int max_numeros;

    while (dividendo>=divisor) {
        resto=dividendo % divisor;
        cadenaInt[i++]=resto;
        dividendo=dividendo/divisor;
    }
    cadenaInt[i++]=dividendo;
    max_numeros=i;

    for (i=max_numeros-1;i>=0;i--) {
        if (cadenaInt[i]==0) strcat(cadenaText,"0");
        if (cadenaInt[i]==1) strcat(cadenaText,"1");
        if (cadenaInt[i]==2) strcat(cadenaText,"2");
        if (cadenaInt[i]==3) strcat(cadenaText,"3");
        if (cadenaInt[i]==4) strcat(cadenaText,"4");
        if (cadenaInt[i]==5) strcat(cadenaText,"5");
        if (cadenaInt[i]==6) strcat(cadenaText,"6");
        if (cadenaInt[i]==7) strcat(cadenaText,"7");
        if (cadenaInt[i]==8) strcat(cadenaText,"8");
        if (cadenaInt[i]==9) strcat(cadenaText,"9");
    }
    return (cadenaText);
}

/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*----------------- OPERACIONES RELACIONADAS CON FILESYSTEM -----------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
#define LONG_BUFFER 50

FILE *existeArchivoFilesystem(FILE *filesystem,char *c_path_bloques) {
    filesystem=fopen (c_path_bloques,"rb");
    if (filesystem==NULL) return (NULL); /*Retorna que el archivo NO existe */
    return (filesystem); /*Retorna que el archivo existe */
}

FILE *creacionFilesystem(FILE *filesystem,char *c_path_bloques) {
    uint32_t unBloque32Bits=0;
    filesystem=fopen (c_path_bloques,"w+b");
    if (filesystem==NULL) {
        printf("No fue posible crear el archivo bloques.dat\n");
        return(NULL);
    }
    else {
        for (int i=0;i<262143;i++) fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,filesystem);
        return (filesystem);
    }
}

FILE *abrirFilesystem(FILE *filesystem,char *c_path_bloques) {
    filesystem=fopen (c_path_bloques,"r+b");
    if (filesystem==NULL) return (0);
    else return (filesystem);
}

FILE *iniciarArchivoFilesystem(FILE *filesystem,char *c_path_bloques) {
	filesystem=existeArchivoFilesystem(filesystem,c_path_bloques);
    if (filesystem) {
    	filesystem=abrirFilesystem(filesystem,c_path_bloques);
        if (filesystem) {
            printf("filesystem abierto\n");
            return(filesystem);
        }
        else {
            printf("No se pudo abrir el archivo de Filesystem\n");
            return (NULL);
        }
    }
    else {
    	filesystem=creacionFilesystem(filesystem,c_path_bloques);
        if(filesystem) {
            printf("Archivo de Filesystem creado\n");
            return (filesystem);
        }
        else {
            printf("No se pudo crear el archivo de Filesystem\n");
            return (NULL);
        };
    }
}

/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------- OPERACIONES RELACIONADAS CON ARCHIVO FCB -----------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/

/*Obtiene del archivo fcb el tamanio del archivo del usuario*/
uint32_t tamanio_Archivo_fcb(char *nombreArchivo,char *c_directorio_fcb) {
    FILE *f_arch_fcb;
    char direccionArchivo[100]="";
    char cDatoLeido[LONG_BUFFER];
    char cTamanoArchivo[10]="";
    int i=0;
    int j=0;
    int tope=0;
    uint32_t numeroTamanioArchivo;

    strcpy(direccionArchivo,c_directorio_fcb);
    strcat(direccionArchivo,"/");
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
	printf("El tamaño de archivo en texto es:%s y en numero:%u\n",cTamanoArchivo,numeroTamanioArchivo);
    fclose(f_arch_fcb);
    return(numeroTamanioArchivo);
}

/*Obtiene del archivo fcb el bloque de inicio en la tabla FAT del archivo del usuario*/
uint32_t bloqueInicial_Archivo_fcb(char *nombreArchivo,char *c_directorio_fcb) {
    FILE *f_arch_fcb;
    char direccionArchivo[100]="";

    char cDatoLeido[LONG_BUFFER];
    char cBloqueInicio[4]="";
    int i=0;
    int j=0;
    int tope=0;
    int numeroBloqueIncial;


    strcpy(direccionArchivo,c_directorio_fcb);
    strcat(direccionArchivo,"/");
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

/*Actualiza datos almacenados en archivo fcb, cuando el archivo al que hace referencia tiene una modificación en relación a su tamaño*/
int actualizar_Archivo_fcb(char *nombreArchivo,uint32_t ui32_longMen_datos,uint32_t ui32_entrada_inicial,char *c_directorio_fcb) {
    FILE *f_arch_fcb;
    char direccionArchivo[100]="";
    char cInfo_a_Almacenar[100]="";
    char cNumero[10];

    strcpy(direccionArchivo,c_directorio_fcb);
    strcat(direccionArchivo,"/");
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
    strcat(cInfo_a_Almacenar,itoa_(ui32_longMen_datos,cNumero));
    strcat(cInfo_a_Almacenar,"\n");
    fwrite(cInfo_a_Almacenar,strlen(cInfo_a_Almacenar),1,f_arch_fcb);

    strcpy(cNumero,"");
    strcpy(cInfo_a_Almacenar,"BLOQUE_INICIAL=");
    strcat(cInfo_a_Almacenar,itoa_(ui32_entrada_inicial,cNumero));
    strcat(cInfo_a_Almacenar,"\n");
    fwrite(cInfo_a_Almacenar,strlen(cInfo_a_Almacenar),1,f_arch_fcb);
    /*-------------------------------------------------------*/
    fclose(f_arch_fcb);
    return(1);
}

/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*--------------------- OPERACIONES RELACIONADAS CON FAT --------------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/

/*Determina la existencia del archivo de tabla FAT*/
FILE *existeArchivoFAT(FILE *fat,char *c_path_fat) {
    fat=fopen (c_path_fat,"rb");
    if (fat==NULL) return (NULL); /*Retorn NULL si el archivo no exite*/
    return (fat); /*Retorna el puntero de archivo si este exite*/
}

/*Crea el archivo de tabla FAT*/
FILE *creacionFAT(FILE *fat,char *c_path_fat,uint32_t ui32_tamanio_fat) {
    uint32_t unBloque32Bits=0;
    fat=fopen (c_path_fat,"w+b");
    if (fat==NULL) {
        printf("No fue posible crear el archivo fat.dat\n");
        return(NULL);
    }
    else {
        for (int i=0;i<ui32_tamanio_fat;i++) fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,fat);
        return (fat);
    }
}

/*Abre archivo de tabla FAT*/
FILE *abrirFAT(FILE *fat,char *c_path_fat) {
    fat=fopen (c_path_fat,"r+b");
    if (fat==NULL) return (NULL);
    else return (fat);
}

/*Busca si hay un archivo fat.dat creado, si es así lo abre. De otro modo lo crea y lo abre*/
FILE *iniciarArchivoFAT(FILE *fat,char *c_path_fat,uint32_t ui32_tamanio_fat) {
    if (existeArchivoFAT(fat,c_path_fat)) {
    	fat=abrirFAT(fat,c_path_fat);
        if (fat!=NULL) {
            printf("--> Archivo tabla FAT abierto\n");
            return(fat);
        }
        else {
            printf("--> No se pudo abrir el archivo tabla FAT\n");
            return (NULL);
        }
    }
    else {
    	fat=creacionFAT(fat,c_path_fat,ui32_tamanio_fat);
        if(fat!=NULL) {
            printf("--> Archivo de tabla FAT creado\n");
            return (fat);
        }
        else {
            printf("--> No se pudo crear el archivo de tabla FAT\n");
            return (NULL);
        }
    }
}

/*Función auxiliar para establecer todas las entradas de la tabla FAT a 0*/
FILE *reiniciar_fat(FILE *fat,uint32_t ui32_max_entradas_fat) {
	uint32_t i;
    uint32_t unBloque32Bits=0;

    rewind(fat);
    for (i=0;i<ui32_max_entradas_fat;i++) fwrite(&unBloque32Bits,sizeof(unBloque32Bits),1,fat);
    rewind(fat);
    return(fat);
}

/*Retorna la siguiente entrada de la tabla FAT siguiendo la lista enlazada*/
uint32_t siguiente_entrada_tabla_FAT(FILE *fat,uint32_t ui32_entrada_FAT) {
    uint32_t ui32_data_bloque_tabla_FAT=0;
    printf ("Estoy en siguiente entrada tabla FAT\n");
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

/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*----------------- OPERACIONES RELACIONADAS LOGGER COMMITED-----------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
int committed_logger_CREAR_ARCHIVO(char *nombreArchivo,t_log *logger){
	char mensajeLog[100]="";
    strcpy(mensajeLog,"Crear Archivo: ");
    strcat(mensajeLog,nombreArchivo);
    log_info(logger,mensajeLog);
    return(1);
}

int committed_logger_APERTURA_ARCHIVO(char *nombreArchivo,t_log *logger){
	char mensajeLog[100]="";
    strcpy(mensajeLog,"Abrir Archivo: ");
    strcat(mensajeLog,nombreArchivo);
    log_info(logger,mensajeLog);
    return(1);
}

int committed_logger_TRUNCAR(char *nombreArchivo,t_log *logger,char *c_directorio_fcb){
	char mensajeLog[100]="";
	char numeroConvertido[10]="";
    strcpy(mensajeLog,"Truncar Archivo: ");
    strcat(mensajeLog,nombreArchivo);
    strcat(mensajeLog," - Tamaño: ");
    strcat(mensajeLog,itoa_(tamanio_Archivo_fcb(nombreArchivo,c_directorio_fcb),numeroConvertido));
    strcat(mensajeLog," bytes");
    log_info(logger,mensajeLog);
    return(1);
}

int committed_logger_LECTURA_ARCHIVO(char *nombreArchivo,uint32_t ui32_ptrArchivo,uint32_t ui32_direcMemoria,t_log *logger){
	char mensajeLog[100]="";
	char numeroConvertido[10]="";
    strcpy(mensajeLog,"Leer Archivo: ");
    strcat(mensajeLog,nombreArchivo);
    strcat(mensajeLog," - Puntero: ");
    strcat(mensajeLog,itoa_(ui32_ptrArchivo,numeroConvertido));
    strcat(mensajeLog," - Memoria: ");
    strcat(mensajeLog,"-- Falta resolver --");
    //strcat(mensajeLog,itoa_(ui32_direcMemoria,numeroConvertido));
    log_info(logger,mensajeLog);
    return(1);
}

int committed_logger_ESCRITURA_ARCHIVO(char *nombreArchivo,uint32_t ui32_ptrArchivo,uint32_t ui32_direcMemoria,t_log *logger){
	char mensajeLog[100]="";
	char numeroConvertido[10]="";
    strcpy(mensajeLog,"Escribir Archivo: ");
    strcat(mensajeLog,nombreArchivo);
    strcat(mensajeLog," - Puntero: ");
    strcat(mensajeLog,itoa_(ui32_ptrArchivo,numeroConvertido));
    strcat(mensajeLog," - Memoria: ");
    strcat(mensajeLog,"-- Falta resolver --");
    //strcat(mensajeLog,itoa_(ui32_direcMemoria,numeroConvertido));
    log_info(logger,mensajeLog);
    return(1);
}

int committed_logger_ACCESO_FAT(uint32_t ui32_numEntrada,uint32_t ui32_dataEntrada,t_log *logger){
	char mensajeLog[100]="";
	char numeroConvertido[10]="";
    strcpy(mensajeLog,"Acceso FAT - Entrada: ");
    strcat(mensajeLog,itoa_(ui32_numEntrada,numeroConvertido));
    strcat(mensajeLog," - Valor: ");
    strcpy(numeroConvertido,"");
    if (ui32_dataEntrada==2499) ui32_dataEntrada=9999;
    strcat(mensajeLog,itoa_(ui32_dataEntrada,numeroConvertido));
    log_info(logger,mensajeLog);
    return(1);
}

int committed_logger_ACCESO_BLOQUE_ARCHIVO(char *nombreArchivo,uint32_t ui32_numBloque_Archivo,uint32_t ui32_numBloque_FS,uint32_t ui32_tamBloque,t_log *logger){
	char mensajeLog[100]="";
	char numeroConvertido[10]="";
    strcpy(mensajeLog,"Acceso Bloque - Archivo: ");
    strcat(mensajeLog,nombreArchivo);
    strcat(mensajeLog," - Bloque Archivo: ");
    strcat(mensajeLog,itoa_(ui32_numBloque_Archivo,numeroConvertido));
    strcat(mensajeLog," - Bloque FS: ");
    strcpy(numeroConvertido,"");
    ui32_numBloque_FS=((64*ui32_tamBloque)+(ui32_numBloque_FS*1024))/sizeof(uint32_t);
    strcat(mensajeLog,itoa_(ui32_numBloque_FS,numeroConvertido));
    log_info(logger,mensajeLog);
    return(1);
}

int committed_logger_ACCESO_BLOQUE_SWAP(char *nombreArchivo,uint32_t ui32_numBloque_Swap,t_log *logger){
	char mensajeLog[100]="";
	char numeroConvertido[10]="";
    strcpy(mensajeLog,"Acceso SWAP: ");
    strcat(mensajeLog,itoa_(ui32_numBloque_Swap,numeroConvertido));
    log_info(logger,mensajeLog);
    return(1);
}


/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*-------------------- OPERACIONES AUXILIARES CON ARCHIVOS ------------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*Función axiliar: Obtiene el tamanio del archivo*/
uint32_t tamanioArchivo (char *nombreArchivo) {
    FILE *f;
    long tamArchivo;
    f=fopen(nombreArchivo,"r");
    fseek(f, 0L, SEEK_END);
    tamArchivo = ftell(f);
    fclose(f);
	printf("El tamanio del archivo:%ld\n",tamArchivo);
    return(tamArchivo);
}

/*Función axiliar: Obtiene el tamanio del archivo*/
int abrirDocumento(char *nombreArchivo,char *documentoArchivo) {
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
/*---------------------------------------------------------------------------------*/
/*-------------------- OPERACIONES CON ARCHIVO BLOQUES.DAT ------------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*--------------- OPERACIONES DIRECTIVAS DEL KERNEL A FILESYSTEM ------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/* 1) ABRIR_ARCHIVO-----------------------------*/
uint32_t d_abrir_archivo(char *nombreArchivo,char *c_directorio_fcb) {
    FILE *f_arch_fcb;
    char direccionArchivo[100]="";

    strcpy(direccionArchivo,c_directorio_fcb);

    strcat(direccionArchivo,nombreArchivo);
    strcat(direccionArchivo,".fcb");
    f_arch_fcb=fopen(direccionArchivo,"r");
    if (f_arch_fcb==NULL) {
        printf("Nombre de archivo incorrecto o no existe");
        exit(1);
    }
    return(tamanio_Archivo_fcb(nombreArchivo,c_directorio_fcb));
}

/* 2) CREAR_ARCHIVO-----------------------------*/
int crear_archivo(char *nombreArchivo,t_log *logger,char *c_directorio_fcb) {
    actualizar_Archivo_fcb(nombreArchivo,0,9999,c_directorio_fcb);
    committed_logger_CREAR_ARCHIVO(nombreArchivo,logger);
    return(1);
}
/* 3) TRUNCAR_ARCHIVO-----------------------------*/
int truncar_archivo(char *nombreArchivo,uint32_t ui32_longMen_datos,t_log *logger,FILE *fat,uint32_t ui32_tamBloque,uint32_t ui32_max_entradas_fat,char *c_directorio_fcb) {
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
    ui32_entrada_inicial=bloqueInicial_Archivo_fcb(nombreArchivo,c_directorio_fcb);

    if (ui32_entrada_inicial==9999) {
        /* Caso (1): Asignacion de entradas a la tabla FAT a un archivo recientemente creado*/
        /*El archivo fcb NO tiene una entrada inicial asignada en la tabla FAT*/
        ui32_cant_bloques_necesitados=cantBloques_FAT_necesitados(ui32_longMen_datos,ui32_tamBloque);
        ui32_cant_bloques_libres=cantidadBloques_FAT_libres(fat,ui32_max_entradas_fat);
        /*Evalua si la cantidad de entradas en la Tabla FAT son suficientes*/
        if(ui32_cant_bloques_necesitados<=ui32_cant_bloques_libres) {
            printf("Se asignaran las entradas en la tabla FAT\n");
            ui32_entrada_inicial=asignarBloquesFAT(fat,ui32_cant_bloques_necesitados,ui32_max_entradas_fat);
            printf("La entrada_ui32_entrada_inicial es:%u\n",ui32_entrada_inicial);
            actualizar_Archivo_fcb(nombreArchivo,ui32_longMen_datos,ui32_entrada_inicial,c_directorio_fcb);
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
        ui32_cant_bloques_necesitados=cantBloques_FAT_necesitados(ui32_longMen_datos,ui32_tamBloque);
        //- Calcula la cantidad de entradas que ya tiene el archivo asignados previamente
        ui32_cant_bloques_asignados=cantBloques_FAT_necesitados(tamanio_Archivo_fcb(nombreArchivo,c_directorio_fcb),ui32_tamBloque);

        if (ui32_cant_bloques_necesitados<=ui32_cant_bloques_asignados) {
            printf("La cantidad de bloques necesitados por la memoria son:%u\n",ui32_cant_bloques_necesitados);
            printf("La cantidad de ya asignados son:%u\n",ui32_cant_bloques_asignados);
            /*Caso 2A - La cantidad de entradas necesitadas es menor o igual a las que tiene
            ya asignadas el archivo*/
            ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo,c_directorio_fcb); //Entrada inicial a la tabla FAT

            //Recorre la lista enlazada mientras que cuenta las entradas que necesita la version en memoria del archivo
            for (indice=0;indice<ui32_cant_bloques_necesitados;indice++) {
                if (indice==0) ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo,c_directorio_fcb); //Entrada inicial a la tabla FAT
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
            ui32_cant_bloques_necesitados=cantBloques_FAT_necesitados(ui32_longMen_datos,ui32_tamBloque);
            ui32_cant_bloques_libres=cantidadBloques_FAT_libres(fat,ui32_max_entradas_fat);
            ui32_cant_bloques_asignados=cantBloques_FAT_necesitados(tamanio_Archivo_fcb(nombreArchivo,c_directorio_fcb),ui32_tamBloque);
            ui32_cant_bloques_adionales=ui32_cant_bloques_necesitados-ui32_cant_bloques_asignados;
            printf("Cantidad de entradas FAT que necesita el documento en memoria son:%u\n",ui32_cant_bloques_necesitados);
            printf("Cantidad de entradas FAT ya asignadas al documento almacenado:%u\n",ui32_cant_bloques_asignados);
            printf("Cantidad de entradas FAT adicionales necesitadas:%u\n",ui32_cant_bloques_adionales);
            printf("Cantidad de entradas FAT libres en la tabla FAT:%u\n",ui32_cant_bloques_libres);

            //Recorre la lista de entradas ya asignadas al archivo, en busqueda de la ultima entrada
            ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo,c_directorio_fcb); //Entrada inicial a la tabla FAT
            ui32_entrada_FAT_siguiente=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
            //printf("entrada_FAT:%u\n",ui32_entrada_FAT);
            //printf("entrada_FAT_siguiente:%u\n",ui32_entrada_FAT_siguiente);
            while ((ui32_entrada_FAT_siguiente!=9999) && (ui32_entrada_FAT_siguiente!=2499)) {
                ui32_entrada_FAT=ui32_entrada_FAT_siguiente;
                ui32_entrada_FAT_siguiente=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
                //printf("entrada_FAT:%u\n",ui32_entrada_FAT);
                //printf("entrada_FAT_siguiente:%u\n",ui32_entrada_FAT_siguiente);
            }
            ui32_data_entrada=asignarBloquesFAT(fat,ui32_cant_bloques_adionales,ui32_max_entradas_fat)*4;
            actualizar_entrada_FAT(fat,ui32_entrada_FAT,ui32_data_entrada);
        }
    }
    committed_logger_TRUNCAR(nombreArchivo,logger,c_directorio_fcb);
    return(1);
}




/* 4) LEER_ARCHIVO-----------------------------*/
int leer_archivo(char *nombreArchivo,t_log *logger,FILE *fat,uint32_t ui32_tamBloque,FILE *filesystem,char *c_directorio_fcb) {
    uint32_t ui32_cantBloques_a_leer=0;
    uint32_t ui32_entrada_FAT;
    uint32_t ui32_dataEntrada_FAT;
    uint32_t ui32_numero_bloque=0;
    uint32_t indice=0;
    char buffer_lectura[1024+1]="";
    char buffer_documento[1024*30]=""; //Tamanio del buffer es un punto a analizar porque no esta determinado

    printf("OP:LEER_ARCHIVO\n");
    /*Calcula la cantidad de bloques a escribir en el archivo bloques.dat*/
    /*Obtiene la direccion de la entrada inicial de la tabla FAT*/
    ui32_cantBloques_a_leer=cantBloques_FAT_necesitados(tamanio_Archivo_fcb(nombreArchivo,c_directorio_fcb),ui32_tamBloque);
    ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo,c_directorio_fcb);
    printf("La entrada inicial en la FAT del archivo:%s es:%u\n",nombreArchivo,ui32_entrada_FAT);
    for (indice=0;indice<ui32_cantBloques_a_leer;indice++) {
        strcat(buffer_documento,lectura_de_archivo_bloques(filesystem,ui32_entrada_FAT,buffer_lectura,ui32_tamBloque));
        ui32_dataEntrada_FAT=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
        printf ("---------------------------------------------------\n");
        printf("La siguiente entrada en la FAT del archivo:%s es:%u\n",nombreArchivo,ui32_entrada_FAT);
        committed_logger_ACCESO_FAT(ui32_entrada_FAT,ui32_dataEntrada_FAT,logger);
        committed_logger_LECTURA_ARCHIVO(nombreArchivo,ui32_entrada_FAT,0,logger);//0 porque falta completar la direccion de memoria
        committed_logger_ACCESO_BLOQUE_ARCHIVO(nombreArchivo,ui32_numero_bloque,ui32_entrada_FAT,ui32_tamBloque,logger);
        ui32_entrada_FAT=ui32_dataEntrada_FAT;
        ui32_numero_bloque++;
    }
    printf("---------------------------------------------------------------------------------\n");
    printf("El documento leido desde el arhivo: %s es:...\n\n",nombreArchivo);
    printf("%s\n",buffer_documento);
    printf("---------------------------------------------------------------------------------\n");
    printf ("El tamanio del archivo es:%ld bytes\n",strlen(buffer_documento));
    printf("---------------------------------------------------------------------------------\n");
    return(1);
}

/* 5) ESCRIBIR_ARCHIVO-----------------------------*/
int escribir_archivo(char *nombreArchivo,char *documentoArchivo,t_log *logger,FILE *fat,uint32_t ui32_tamBloque,FILE *filesystem,char *c_directorio_fcb) {
    uint32_t ui32_cantBloques_a_escribir=0;
    uint32_t ui32_entrada_FAT=0;
    uint32_t ui32_dataEntrada_FAT;
    uint32_t ui32_numero_bloque=0;
    char buffer_escritura[1024];
    uint32_t indice=0;

    printf("OP:ESCRIBIR_ARCHIVO\n");
    /*Calcula la cantidad de bloques a escribir en el archivo bloques.dat*/
    /*Obtiene la direccion de la entrada inicial de la tabla FAT*/
    ui32_cantBloques_a_escribir=cantBloques_FAT_necesitados(tamanio_Archivo_fcb(nombreArchivo,c_directorio_fcb),ui32_tamBloque);
    ui32_entrada_FAT=bloqueInicial_Archivo_fcb(nombreArchivo,c_directorio_fcb);
    for (indice=0;indice<ui32_cantBloques_a_escribir;indice++) {
        strcpy(buffer_escritura,cargar_buffer_escritura(buffer_escritura,ui32_numero_bloque,documentoArchivo,ui32_tamBloque));
        escribir_en_archivo_bloques(filesystem,ui32_entrada_FAT,buffer_escritura,ui32_tamBloque);
        ui32_dataEntrada_FAT=siguiente_entrada_tabla_FAT(fat,ui32_entrada_FAT);
        committed_logger_ACCESO_FAT(ui32_entrada_FAT,ui32_dataEntrada_FAT,logger);
        committed_logger_ESCRITURA_ARCHIVO(nombreArchivo,ui32_entrada_FAT,0,logger);//0 porque falta completar la direccion de memoria
        committed_logger_ACCESO_BLOQUE_ARCHIVO(nombreArchivo,ui32_numero_bloque,ui32_entrada_FAT,ui32_tamBloque,logger);
        ui32_entrada_FAT=ui32_dataEntrada_FAT;
        ui32_numero_bloque++;
    }
    return(1);
}


/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*------------------------------------ main() -------------------------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/


int main(int argc, char* argv[]) {
    uint32_t ui32_tam_de_archivo=0;
    uint32_t ui32_max_entradas_fat;
    uint32_t ui32_cant_bloques_total=0;
	uint32_t ui32_cant_bloques_swap;
	uint32_t ui32_tam_bloque;

	FILE *filesystem;
	FILE *fcb;
	FILE *fat;

    char *c_direccion_ip_filesystem="127.0.0.1";



	//------------------------------------------------------------------------
    t_log* logger = iniciar_logger("log_filesystem.log","FILESYSTEM");
    t_config* config = iniciar_config("/home/utnso/Desktop/Compartido_Windows/tp-2023-2c-los-operativos/filesystem/cfg/filesystem.config");
    
	char* puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    char* ip_memoria = config_get_string_value(config,"IP_MEMORIA");
    char* puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	char* path_fat = config_get_string_value(config,"PATH_FAT");
    char* path_bloques = config_get_string_value(config,"PATH_BLOQUES");
	char* path_fcb = config_get_string_value(config,"PATH_FCB");
	char* cant_bloques_total=config_get_string_value(config,"CANT_BLOQUES_TOTAL");
    char* cant_bloques_swap=config_get_string_value(config,"CANT_BLOQUES_SWAP");
    char* tam_bloque=config_get_string_value(config,"TAM_BLOQUE");
    //char* retardo_acceso_bloque=config_get_string_value(config,"RETARDO_ACCESO_BLOQUE");
    //char* retardo_acceso_fat=config_get_string_value(config,"RETARDO_ACCESO_FAT");

    ui32_cant_bloques_total=atoi_(cant_bloques_total);
    ui32_cant_bloques_swap=atoi_(cant_bloques_swap);
    ui32_max_entradas_fat=ui32_cant_bloques_total-ui32_cant_bloques_swap;
    ui32_tam_bloque=atoi_(tam_bloque);

    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha);

    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);

    int socket_servidor = iniciar_servidor(logger, puerto_escucha);
    int socket_kernel = esperar_cliente(logger, socket_servidor);
    if(socket_kernel){
        log_info(logger,"Se conectó kernel");
    }
    //Creación e inicialización de Filesystem y Fat
    filesystem=iniciarArchivoFilesystem(filesystem,path_bloques);
    fat=iniciarArchivoFAT(fat,path_fat,ui32_max_entradas_fat);

    //Configuraciones temporales para realizar pruebas
    fat=reiniciar_fat(fat,ui32_max_entradas_fat); //(QUIRAR A FUTUTO)Reset de fat.dat para probar la creación de la tabla
	filesystem=creacionFilesystem(filesystem,path_bloques); //(QUIRAR A FUTUTO)reset temporal de archivo bloques.dat
    
	//Documento para probar el funcionamiento del Filesystem y Fat
    ui32_tam_de_archivo=abrirDocumento("documento1.txt",documentoArchivo);
	printf ("-------------------------------------------efewfwefwefwefew\n");
    op_code operacion;

    while(1)
    {
        operacion = recibir_operacion(socket_kernel);
		printf ("\nQue es lo que contiene operacion:%d\n",operacion);
		if (operacion==0) {
			//char* nombre = recibir_mensaje(socket_kernel);
			//uint32_t tam_archivo = abrir_archivo(nombre, path_fcb);
		}
		else if (operacion==5000){
			printf ("Debería crear el filesystem\n");
			creacionFilesystem(filesystem,path_bloques);
		}
		else if (operacion==5001){
			reiniciar_fat(fat,ui32_max_entradas_fat);
		}
		else if (operacion==5002){
			abrir_archivo("documento1",path_fcb);
		}
		else if (operacion==5003){
			crear_archivo("documento1",logger,path_fcb);
		}
		else if (operacion==5004){
			truncar_archivo("documento1",ui32_tam_de_archivo,logger,fat,ui32_tam_bloque,ui32_max_entradas_fat,path_fcb);
		}
		else if (operacion==5005){
			leer_archivo("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb);
		}
		else if (operacion==5006){
			escribir_archivo("documento1",documentoArchivo,logger,fat,ui32_tam_bloque,filesystem,path_fcb);
		}
		else if (operacion==5007){
			printf ("Debería mostrar la tabla FAT\n");
			mostrar_tabla_FAT(fat,ui32_max_entradas_fat);
		}
		else if (operacion==5008){
		    fclose(fat);
		    fclose(filesystem);
		    return EXIT_SUCCESS;
		}
		else {
			liberar_conexion(socket_kernel);
			return;
        }
    }
    return 0;
}
