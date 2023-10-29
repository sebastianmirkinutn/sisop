#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>



t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	return nuevo_config;
}

t_config *startUpConfig(t_config *config) {
	config = iniciar_config();
	chdir("chdir(/Filesystem2/src/");
	config=config_create("filesystem.config");
    if (config==NULL) {
	    exit(0);
	    //a completar con código logger
	}
    else {
    	//a completar con código logger
    }
	return (config);
}


char *leerConfiguracion_ip_memoria(t_config *config) {
	char *ip_memoria;
	char mensaje[60]="";

	if (config_has_property(config,"IP_MEMORIA")) {
		ip_memoria=config_get_string_value(config,"IP_MEMORIA");
		strcat(mensaje,"Encontró IP_MEMORIA=");
		strcat(mensaje,ip_memoria);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
		strcpy(mensaje,""); //limpia mensaje para próxima lectura
	}
	return(ip_memoria);
}

char *leerConfiguracion_puerto_memoria(t_config *config) {
	char *puerto_memoria;
	char mensaje[60]="";

	if (config_has_property(config,"PUERTO_MEMORIA")) {
		puerto_memoria=config_get_string_value(config,"PUERTO_MEMORIA");
		strcat(mensaje,"Encontró PUERTO_MEMORIA=");
		strcat(mensaje,puerto_memoria);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
		strcpy(mensaje,""); //limpia mensaje para próxima lectura
	}
	return(puerto_memoria);
}

char *leerConfiguracion_puerto_escucha(t_config *config) {
	char *puerto_escucha;
	char mensaje[60]="";

	if (config_has_property(config,"PUERTO_ESCUCHA")) {
		puerto_escucha=config_get_string_value(config,"PUERTO_ESCUCHA");
		strcat(mensaje,"Encontró PUERTO_ESCUCHA=");
		strcat(mensaje,puerto_escucha);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
		strcpy(mensaje,""); //limpia mensaje para próxima lectura
	}
	return(puerto_escucha);
}

char *leerConfiguracion_path_fat(t_config *config) {
	char *path_fat;
	char mensaje[200]="";

	if (config_has_property(config,"PATH_FAT")) {
		path_fat=config_get_string_value(config,"PATH_FAT");
		strcat(mensaje,"Encontró PATH_FAT=");
		strcat(mensaje,path_fat);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
	}
	return(path_fat);
}

char *leerConfiguracion_path_bloques(t_config *config) {
	char *path_bloques;
	char mensaje[200]="";

	if (config_has_property(config,"PATH_BLOQUES")) {
		path_bloques=config_get_string_value(config,"PATH_BLOQUES");
		strcat(mensaje,"Encontró PATH_BLOQUES=");
		strcat(mensaje,path_bloques);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
	}
	return(path_bloques);
}


char *leerConfiguracion_path_fcb(t_config *config) {
	char *path_fcb;
	char mensaje[200]="";

	if (config_has_property(config,"PATH_FCB")) {
		path_fcb=config_get_string_value(config,"PATH_FCB");
		strcat(mensaje,"Encontró PATH_FCB=");
		strcat(mensaje,path_fcb);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
	}
	return(path_fcb);
}


char *leerConfiguracion_cant_bloques_total(t_config *config) {
	char *cant_bloques_total;
	char mensaje[60]="";

	if (config_has_property(config,"CANT_BLOQUES_TOTAL")) {
		cant_bloques_total=config_get_string_value(config,"CANT_BLOQUES_TOTAL");
		strcat(mensaje,"Encontró CANT_BLOQUES_TOTAL=");
		strcat(mensaje,cant_bloques_total);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
	}
	return(cant_bloques_total);
}

char *leerConfiguracion_cant_bloques_swap(t_config *config) {
	char *cant_bloques_swap;
	char mensaje[60]="";

	if (config_has_property(config,"CANT_BLOQUES_SWAP")) {
		cant_bloques_swap=config_get_string_value(config,"CANT_BLOQUES_SWAP");
		strcat(mensaje,"Encontró CANT_BLOQUES_SWAP=");
		strcat(mensaje,cant_bloques_swap);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
	}
	return(cant_bloques_swap);
}

char *leerConfiguracion_tam_bloque(t_config *config) {
	char *tam_bloque;
	char mensaje[60]="";
	if (config_has_property(config,"TAM_BLOQUE")) {
		tam_bloque=config_get_string_value(config,"TAM_BLOQUE");
		strcat(mensaje,"Encontró TAM_BLOQUE=");
		strcat(mensaje,tam_bloque);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
	}
	return(tam_bloque);
}

char *leerConfiguracion_retardo_acceso_bloque(t_config *config) {
	char *retardo_acceso_bloque;
	char mensaje[60]="";

	if (config_has_property(config,"RETARDO_ACCESO_BLOQUE")) {
		retardo_acceso_bloque=config_get_string_value(config,"RETARDO_ACCESO_BLOQUE");
		strcat(mensaje,"Encontró RETARDO_ACCESO_BLOQUE=");
		strcat(mensaje,retardo_acceso_bloque);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
	}
	return(retardo_acceso_bloque);
}


char *leerConfiguracion_retardo_acceso_fat(t_config *config) {
	char *retardo_acceso_fat;
	char mensaje[60]="";
	if (config_has_property(config,"RETARDO_ACCESO_FAT")) {
		retardo_acceso_fat=config_get_string_value(config,"RETARDO_ACCESO_FAT");
		strcat(mensaje,"Encontró RETARDO_ACCESO_FAT=");
		strcat(mensaje,retardo_acceso_fat);
		printf ("%s\n",mensaje);
		//log_info(logger,mensaje);
	}
	return(retardo_acceso_fat);
}
