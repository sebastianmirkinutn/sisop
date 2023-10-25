#include "../include/main.h"

t_registros* registros;

sem_t mutex_flag_interrupciones;

typedef struct
{
    int socket_interrupt;
    int socket_dispatch;
}t_args_hilo;

int flag_interrupciones;
int execute;

void recibir_interrupciones(void* arg)
{
    op_code operacion;
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    
    while (1)
    {
        operacion = recibir_operacion(arg_h->socket_interrupt);
        if(operacion == INTERRUPT) //Acá podemos diferenciar el tipo de interrupción.
        {
            sem_wait(&mutex_flag_interrupciones);
            flag_interrupciones = 1; 
            sem_post(&mutex_flag_interrupciones);
        }
    }
}

void atender_interrupciones(int socket_kernel_dispatch)
{
    sem_wait(&mutex_flag_interrupciones);
    if(flag_interrupciones)
    {
        flag_interrupciones = 0; 
        sem_post(&mutex_flag_interrupciones);
        enviar_desalojo(socket_kernel_dispatch, CLOCK_INTERRUPT);
        send(socket_kernel_dispatch, &(registros->AX), sizeof(uint32_t), 0);
        send(socket_kernel_dispatch, &(registros->BX), sizeof(uint32_t), 0);
        send(socket_kernel_dispatch, &(registros->CX), sizeof(uint32_t), 0);
        send(socket_kernel_dispatch, &(registros->DX), sizeof(uint32_t), 0);
        send(socket_kernel_dispatch, &(registros->PC), sizeof(uint32_t), 0);
        execute = 0;
    }
    else
    {
        sem_post(&mutex_flag_interrupciones);
    }
}

void sumar_a_registro(char* registro, uint32_t numero)
{
    if(!strcmp(registro, "AX"))
    {
        registros->AX += numero;
    }
    else if(!strcmp(registro, "BX"))
    {
        registros->BX += numero;
    }
    else if(!strcmp(registro, "CX"))
    {
        registros->BX += numero;
    }
    else if(!strcmp(registro, "CX"))
    {
        registros->CX += numero;
    }
    else if(!strcmp(registro, "DX"))
    {
        registros->DX += numero;
    }
}

void sum(char* destino, char* origen){
    if(!strcmp(origen, "AX"))
    {
        sumar_a_registro(destino, registros->AX);
    }
    else if(!strcmp(origen, "BX"))
    {
        sumar_a_registro(destino, registros->BX);
    }
    else if(!strcmp(origen, "CX"))
    {
        sumar_a_registro(destino, registros->CX);
    }
    else if(!strcmp(origen, "DX"))
    {
        sumar_a_registro(destino, registros->DX);
    }
}

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
    /*
    if(strcmp(recibir_mensaje(conexion_memoria), "LISTO_PARA_RECIBIR_PEDIDOS"))
    {
        log_error(logger, "No se pudo establecer la conexión con Memoria");
    }
    */
    
    uint32_t pid = 0;
    
    registros = malloc(sizeof(t_registros));
    flag_interrupciones = 0;
    sem_init(&mutex_flag_interrupciones, 0, 1);

    t_args_hilo args_conexion_kernel;
    args_conexion_kernel.socket_dispatch = socket_kernel_dispatch;
    args_conexion_kernel.socket_interrupt = socket_kernel_interrupt; 
    pthread_t hilo_interrupt_handler;
    pthread_create(&hilo_interrupt_handler, NULL, &recibir_interrupciones, (void*)&args_conexion_kernel);
    pthread_detach(&hilo_interrupt_handler);
    while(1)
    {
        execute = 1;

        recv(socket_kernel_dispatch, &pid, sizeof(uint32_t), MSG_WAITALL);
        log_info(logger, "recibí pid %i", pid);
        //registros = recibir_contexto_de_ejecucion(socket_kernel_dispatch);
        recv(socket_kernel_dispatch, &(registros->AX), sizeof(uint32_t), MSG_WAITALL);
        recv(socket_kernel_dispatch, &(registros->BX), sizeof(uint32_t), MSG_WAITALL);
        recv(socket_kernel_dispatch, &(registros->CX), sizeof(uint32_t), MSG_WAITALL);
        recv(socket_kernel_dispatch, &(registros->DX), sizeof(uint32_t), MSG_WAITALL);
        recv(socket_kernel_dispatch, &(registros->PC), sizeof(uint32_t), MSG_WAITALL);
        t_pcb* pcb_prueba = crear_pcb(1,"");
        pcb_prueba->contexto = registros;
        log_info(logger, "AX:%i - BX:%i - CX:%i - DX:%i - PC:%i", pcb_prueba->contexto->AX, pcb_prueba->contexto->BX, pcb_prueba->contexto->CX, pcb_prueba->contexto->DX, pcb_prueba->contexto->PC);
        
        /*CICLO DE INSTRUCCIÓN*/
        while(execute)
        {
            /*FETCH*/
            op_code codigo = FETCH_INSTRUCCION;
            send(conexion_memoria, &codigo, sizeof(op_code), 0);
            send(conexion_memoria, &pid, sizeof(uint32_t), 0);
            send(conexion_memoria, &(registros->PC), sizeof(uint32_t), 0) ;  
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
            /*Uso ifs anidados en lugar de pasar a enum y usar switch porque, independientemente de que
            usemos un enum, vamos a tener que hacer strcmp en ifs anidados.*/
            if(!strcmp(parametros[0], "SET"))
            {
                if(!strcmp(parametros[1], "AX"))
                {
                    registros->AX = atoi(parametros[2]);
                }
                else if(!strcmp(parametros[1], "BX"))
                {
                    registros->BX = atoi(parametros[2]);
                }
                else if(!strcmp(parametros[1], "CX"))
                {
                    registros->CX = atoi(parametros[2]);
                }
                else if(!strcmp(parametros[1], "DX"))
                {
                    registros->DX = atoi(parametros[2]);
                }
                //log_info(logger, "AX:%i - BX:%i - CX:%i - DX:%i - PC:%i", pcb_prueba->contexto->AX, pcb_prueba->contexto->BX, pcb_prueba->contexto->CX, pcb_prueba->contexto->DX, pcb_prueba->contexto->PC);
            }
            else if(!strcmp(parametros[0], "SUM"))
            {
                sum(parametros[1], parametros[2]);
            }
            else if(!strcmp(parametros[0], "SUB"))
            {
            
            }
            else if(!strcmp(parametros[0], "JNZ"))
            {
            
            }
            else if(!strcmp(parametros[0], "SLEEP"))
            {
                sleep(atoi(parametros[1]));
            }
            else if(!strcmp(parametros[0], "WAIT"))
            {
            
            }
            else if(!strcmp(parametros[0], "SIGNAL"))
            {
            
            }
            else if(!strcmp(parametros[0], "MOV_IN"))
            {
            
            }
            else if(!strcmp(parametros[0], "MOV_OUT"))
            {
            
            }
            else if(!strcmp(parametros[0], "F_OPEN"))
            {
            
            }
            else if(!strcmp(parametros[0], "F_CLOSE"))
            {
            
            }
            else if(!strcmp(parametros[0], "F_SEEK"))
            {
            
            }
            else if(!strcmp(parametros[0], "F_READ"))
            {
            
            }
            else if(!strcmp(parametros[0], "F_WRITE"))
            {
            
            }
            else if(!strcmp(parametros[0], "F_TRUNCATE"))
            {
            
            }
            else if(!strcmp(parametros[0], "EXIT"))
            {
                execute = 0;
                registros->PC = 0;
                enviar_desalojo(socket_kernel_dispatch, SUCCESS);
                send(socket_kernel_dispatch, &(registros->AX), sizeof(uint32_t), 0);
                send(socket_kernel_dispatch, &(registros->BX), sizeof(uint32_t), 0);
                send(socket_kernel_dispatch, &(registros->CX), sizeof(uint32_t), 0);
                send(socket_kernel_dispatch, &(registros->DX), sizeof(uint32_t), 0);
                send(socket_kernel_dispatch, &(registros->PC), sizeof(uint32_t), 0);
            }
            registros->PC++;
            atender_interrupciones(socket_kernel_dispatch);
        }
    }
}