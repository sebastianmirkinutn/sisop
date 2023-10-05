#include "../include/main.h"

int main(int argc, char* argv[]){
    t_log* logger = iniciar_logger("log_cpu.log","CPU");
    t_config* config = iniciar_config("./cfg/cpu.config");
    char* puerto_escucha;
    char* puerto_escucha_dispatch = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
    char* puerto_escucha_interrupt = config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
    char* ip_memoria = config_get_string_value(config,"IP_MEMORIA");
    char* puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha);

    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);

    int socket_servidor_dispatch = iniciar_servidor(logger,puerto_escucha_dispatch);
    int socket_servidor_interrupt = iniciar_servidor(logger,puerto_escucha_interrupt);
    int socket_kernel_dispatch = esperar_cliente(logger, socket_servidor_dispatch);
    if(socket_kernel_dispatch){
        log_info(logger,"Se conectó kernel al puerto dispatch");
    }
    int socket_kernel_interrupt = esperar_cliente(logger, socket_servidor_interrupt);
    if(socket_kernel_interrupt){
        log_info(logger,"Se conectó kernel al puerto interrupt");
    }
    //recibir_mensaje(logger, socket_kernel_dispatch);
    while(1){
        char* funcion = recibir_mensaje(logger, socket_kernel_dispatch);
        char* parametros[3];
        parametros[0] = recibir_mensaje(logger, socket_kernel_dispatch);
        parametros[1] = recibir_mensaje(logger, socket_kernel_dispatch);
        parametros[2] = recibir_mensaje(logger, socket_kernel_dispatch);
        log_info(logger, "%s %s %s %s", funcion, parametros[0], parametros[1], parametros[2]);
    }
}