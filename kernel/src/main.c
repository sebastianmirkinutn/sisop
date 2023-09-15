#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include "./../../shared/src/client_utils.c"
#include "./../../shared/src/log_utils.c"

int main(int argc, char* argv[]){
    t_log* logger = iniciar_logger("log_kernel","Kernel");
    t_config* config = iniciar_config("./cfg/kernel.config");
    
    char* ip_cpu = config_get_string_value(config, "IP_CPU");
	char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");

    int conexion_cpu = crear_conexion(logger, ip_cpu, puerto_cpu);
    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);


    enviar_mensaje("Mensaje", conexion_cpu);

    log_destroy(logger);
    config_destroy(config);
    liberar_conexion(conexion_cpu);
    
}