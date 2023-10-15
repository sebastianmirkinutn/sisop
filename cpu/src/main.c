#include "../include/main.h"

t_registros* registros;
uint32_t program_counter;

int main(int argc, char* argv[]){
    t_log* logger = iniciar_logger("log_cpu.log","CPU");
    t_config* config = iniciar_config("./cfg/cpu.config");
    char* puerto_escucha_dispatch = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
    char* puerto_escucha_interrupt = config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
    char* ip_memoria = config_get_string_value(config,"IP_MEMORIA");
    char* puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha_dispatch);

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
    if(strcmp(recibir_mensaje(conexion_memoria), "LISTO_PARA_RECIBIR_PEDIDOS"))
    {
        log_error(logger, "No se pudo establecer la conexión con Memoria");
    }
    
    uint32_t pid = 0;

    t_pcb* pcb_prueba = crear_pcb(1,"");
    printf("%s", recibir_mensaje(socket_kernel_dispatch));
    printf("Voy a recibir el contexto - socket %i\n", socket_kernel_dispatch);
    pcb_prueba->contexto = recibir_contexto_de_ejecucion(socket_kernel_dispatch);
    log_info(logger, "AX:%i - BX:%i - CX:%i - DX:%i - PC:%i", pcb_prueba->contexto->AX, pcb_prueba->contexto->BX, pcb_prueba->contexto->CX, pcb_prueba->contexto->DX, pcb_prueba->contexto->PC);

    while(1)
    {
        recv(socket_kernel_dispatch, &pid, sizeof(uint32_t), MSG_WAITALL);
        registros = recibir_contexto_de_ejecucion(socket_kernel_dispatch);
        log_info(logger, "recibí pid %i", pid);
        /*CICLO DE INSTRUCCIÓN*/
        while(1)
        {
            /*FETCH*/
            op_code codigo = FETCH_INSTRUCCION;
            send(conexion_memoria, &codigo, sizeof(op_code), 0);
            send(conexion_memoria, &pid, sizeof(uint32_t), 0);
            send(conexion_memoria, &program_counter, sizeof(uint32_t), 0) ;  
            log_info(logger, "Envié el pedido");     
            char* instruccion = recibir_mensaje(conexion_memoria);
            log_info(logger, "%s", instruccion);

            /*DECODE*/
            char* parametros[4];
            {
                char* token;
                int i = 0;
                token = strtok(instruccion, " ");
                while(token != NULL && i < 4)
                {
                    parametros[i] = strdup(token);
                    token = strtok(NULL, " ");
                    i++;
                }
            }
            log_info(logger, "%s", parametros[0]);
            log_info(logger, "%s", parametros[1]);
            log_info(logger, "%s", parametros[2]);
            log_info(logger, "%s %s %s", parametros[0], parametros[1], parametros[2]);

            /*EXECUTE*/
            if(!strcmp(parametros[0], "EXIT"))
            {
                enviar_contexto(registros, socket_kernel_dispatch);
                enviar_desalojo(socket_kernel_dispatch, SUCCESS);
            }

            program_counter++;
        }
    }
}