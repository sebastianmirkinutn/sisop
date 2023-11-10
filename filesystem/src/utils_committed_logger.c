#include "utils_committed_logger.h"

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