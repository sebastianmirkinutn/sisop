#include "../include/main.h"

t_instruccion* parserar_argumentos(char* str)
{
    if(str == NULL)
    {
       return NULL;
    }
    t_instruccion* instruccion = malloc(instruccion);
    t_list* argumentos = list_create();
    char* str_cpy = strdup(str);
    char* token = strtok(str_cpy, " ");
    instruccion->operacion = token;
    token = strtok(NULL, " ");
    while(token != NULL)
    {
        if (strlen(token) > 0)
        {
            char* token_cpy = strdup(token);
            token_cpy[strlen(token_cpy)] = '\0';
            list_add(argumentos, token_cpy);
        }
        token = strtok(NULL, " ");
    }
    return instruccion;
}

void parsear_instrucciones(t_proceso* proceso, char* str)
{
    if(str == NULL)
    {
       return NULL;
    }
    //t_list* instrucciones = list_create();
    char* str_cpy = strdup(str);
    char* token = strtok(str_cpy, "\n");
    while(token != NULL)
    {
        if (strlen(token) > 0)
        {
            char* token_cpy = strdup(token);
            token_cpy[strlen(token_cpy)] = '\0';
            list_add(proceso->instrucciones, (parserar_argumentos(token_cpy)));
            //log_info(logger, token);
        }
        token = strtok(NULL, "\n");
    }
}

char* leer_pseudocodigo(char* nombre_archivo)
{
    char* ruta = "./pseudocodigo/";
    char* pseudocodigo;
    strcat(ruta, nombre_archivo);
    free(nombre_archivo);
    FILE* archivo = fopen(ruta, "R");
    fseek(archivo, 0, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    fread(archivo, sizeof(char), 1, pseudocodigo);
    return pseudocodigo;
}
/*
int recibir_ruta_pseudocodigo(int socket)
{
    int size;
    char* ruta;
    recv(socket, &size, sizeof(int), MSG_WAITALL);
    recv(socket, ruta, size, MSG_WAITALL);
    return ruta;

}
*/
t_proceso* crear_proceso(uint32_t pid)
{
    t_proceso* proceso = malloc(sizeof(t_proceso));
    proceso->pid = pid;
    proceso->instrucciones = list_create();
}

void conexion_kernel(void* arg)
{
    t_log* logger_hilo = iniciar_logger("logger_hilo.log","HILO");
    log_info(logger_hilo, "HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo,"Socket: %i", arg_h->socket);
    while(1)
    {
        op_code codigo = recibir_operacion(arg_h->socket);
        log_info(logger_hilo,"op_code: %i", codigo);
        switch (codigo)
        {
        case INICIAR_PROCESO:
            int pid;
            recv(socket, &pid, sizeof(int), MSG_WAITALL);
            log_info(logger_hilo,"pid: %i", pid);
            //t_proceso* proceso = crear_proceso(pid);
            //char* ruta = recibir_mensaje(socket);
            //parsear_instrucciones(proceso, leer_pseudocodigo(ruta));
            //log_info(logger_hilo, list_get(proceso->instrucciones,0));
            break;
        
        default:
            //break;
        }

    }
    
}

void conexion_cpu(int socket)
{
    while(1)
    {
        
    }
}

int main(int argc, char* argv[]){
    t_log* logger = iniciar_logger("log_memoria.log","CPU");
    t_config* config = iniciar_config("./cfg/memoria.config");
    char* puerto_escucha;
    puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha);

    int socket_servidor = iniciar_servidor(logger,puerto_escucha);
    int socket_filesystem = esperar_cliente(logger, socket_servidor);
    if(socket_filesystem){
        log_info(logger,"Se conectó filesystem");
    }
    int socket_cpu = esperar_cliente(logger, socket_servidor);
    if(socket_cpu){
        log_info(logger,"Se conectó cpu");
    }
    int socket_kernel = esperar_cliente(logger, socket_servidor);
    if(socket_kernel){
        log_info(logger,"Se conectó kernel");
    }

    t_args_hilo args_conexion_kernel;
    args_conexion_kernel.socket = socket_kernel;
    pthread_t hilo_conexion_kernel;
    log_info(logger, "Declaré el hilo.");
    log_info(logger, "socket: %i", args_conexion_kernel.socket);
    pthread_create(&hilo_conexion_kernel, NULL, &conexion_kernel, (void*)&args_conexion_kernel);
    log_info(logger, "Creé el hilo.");
    pthread_join(&hilo_conexion_kernel,NULL);
    //pthread_detach(&hilo_conexion_kernel);
    //while(1);
    
}