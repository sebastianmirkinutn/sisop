#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	if((nuevo_logger = log_create("tp0.log", "PROCESS_TP0", 1, LOG_LEVEL_INFO)) == NULL){
		printf("No se pudo crear el logger.\n");
		exit(2);
	}

	return nuevo_logger;
}

t_config* iniciar_config(char* config)
{
	t_config* nuevo_config;
	if((nuevo_config = config_create(config)) == NULL){
		printf("No se pudo leer la config.\n");
		exit(2);
	}
	return nuevo_config;
}