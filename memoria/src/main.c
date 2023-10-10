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
        }
        token = strtok(NULL, "\n");
    }
}

char* leer_pseudocodigo(t_proceso* proceso, char* nombre_archivo)
{
    char* ruta = "./pseudocodigo/";
    strcat(ruta, nombre_archivo);
    free(nombre_archivo);
    FILE* archivo = fopen(ruta, "R");
    return 
}

int recibir_ruta_pseudocodigo(int socket)
{
    int size;
    char* ruta;
    recv(socket, &size, sizeof(int), 0);
    recv(socket, ruta, size, 0);
    return ruta;

}

t_proceso* crear_proceso(uint32_t pid)
{
    t_proceso* proceso = malloc(sizeof(t_proceso));
    proceso->pid = pid;
    proceso->instrucciones = list_create();
}

void conexion_kernel(int socket)
{
    while(1)
    {
        op_code codigo = recibir_operacion(socket);
        switch (codigo)
        {
        case INICIAR_PROCESO:
            uint32_t pid;
            recv(socket, &pid, sizeof(uint32_t), 0);
            t_proceso* proceso = crear_proceso(pid);
            parsear_instrucciones(proceso, )
            break;
        
        default:
            break;
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
}