#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include "./../../shared/src/log_utils.c"
#include "./../../shared/src/server_utils.c"


int main(int argc, char* argv[]){
    t_log* logger = iniciar_logger("log_cpu.log","CPU");
    t_config* config = iniciar_config("./cfg/cpu.config");
    char* puerto_escucha;
    puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha);

    int socket_servidor = iniciar_servidor(logger,puerto_escucha);
    int socket_kernel = esperar_cliente(logger, socket_servidor);
}