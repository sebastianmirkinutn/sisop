#include "../include/main.h"

/*ESTRUCTURAS PARA MEMORIA DE INSTRUCCIONES*/
t_list* procesos_en_memoria;
char* saveptr;
sem_t mutex_lista_procesos;
sem_t cantidad_de_procesos;

void parsear_instrucciones(t_log* logger,t_proceso* proceso, char* str)
{
    if(str == NULL)
    {
        log_error(logger, "No hay instrucciones.");
        return NULL;
    }
    //t_list* instrucciones = list_create();
    char* str_cpy = strdup(str);
    char* token = strtok(str_cpy, "\n");
    //log_info(logger, "Instruccion: %s", token);
    while(token != NULL)
    {
        log_info(logger, "Instruccion: %s", token);
        if (strlen(token) > 0)
        {
            char* token_cpy = malloc(strlen(token));
            memcpy(token_cpy, token,strlen(token));
            token_cpy[strlen(token_cpy)] = '\0';
            list_add(proceso->instrucciones, token);
            log_info(logger, "Hice list_add de: %s", token);
            free(token_cpy);
        }
        token = strtok(NULL, "\n");
    }
    log_info(logger,"TOKEN NULL");
}

char* leer_pseudocodigo(t_log* logger, char* nombre_archivo)
{
    log_info(logger, "leer_pseudocodigo.");
    char* ruta = malloc(strlen(nombre_archivo) + 15);
    strcpy(ruta, "./pseudocodigo/");
    log_info(logger, "char*.");
    strcat(ruta, nombre_archivo);
    log_info(logger, "strcat.");
    log_info(logger, "ruta: %s", ruta);    
    FILE* archivo = fopen(ruta, "r");
    log_info(logger, "Se abrió el archivo.");
    if(archivo == NULL)
    {
        log_error(logger, "Error al abrir el archivo.");
        return NULL;
    }

    int size;
    char* pseudocodigo;

    fseek(archivo, 0, SEEK_END);
    size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);

    pseudocodigo = malloc(size + 1);

    if(pseudocodigo == NULL)
    {
        log_error(logger, "Error asignando memoria.");
        fclose(archivo);
    }

    fread(pseudocodigo, 1, size, archivo);
    pseudocodigo[size] = '\0';

    fclose(archivo);

    printf("%s", pseudocodigo);
    return pseudocodigo;

}

t_proceso* crear_proceso(uint32_t pid)
{
    t_proceso* proceso = malloc(sizeof(t_proceso));
    proceso->pid = pid;
    proceso->instrucciones = list_create();
    return proceso;
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
            uint32_t pid;
            recv(arg_h->socket, &pid, sizeof(uint32_t), MSG_WAITALL);
            t_proceso* proceso = crear_proceso(pid);
            log_info(logger_hilo,"pid: %i", pid);
            char* ruta = recibir_mensaje(arg_h->socket);
            log_info(logger_hilo, "%s", ruta);
            parsear_instrucciones(logger_hilo, proceso, leer_pseudocodigo(logger_hilo, ruta));
            sem_wait(&mutex_lista_procesos);
            list_add(procesos_en_memoria, proceso);
            sem_post(&mutex_lista_procesos);
            sem_post(&cantidad_de_procesos);
            log_info(logger_hilo, "SIGNAL cantidad_de_procesos");
            break;
        
        default:
            //break;
        }

    }
    
}


//uint32_t pid_funcion;
t_proceso* buscar_proceso(uint32_t pid)
{
    int comparar(void* arg){
        printf("COMPARAR");
        t_proceso* proceso = arg;
        return proceso->pid == pid;
    }
    printf("EMPIEZA BUCAR_PROCESO\n");
    t_proceso* proceso;
     printf("EMPIEZA BUCAR_PROCESO\n");
    sem_wait(&mutex_lista_procesos);
     printf("hice waitss BUCAR_PROCESO\n");
     //pid_funcion = pid;
    proceso = list_find(procesos_en_memoria, (void*)comparar);
    sem_post(&mutex_lista_procesos);
    printf("TERMINA BUCAR_PROCESO\n");
    return proceso;
}

void conexion_cpu(void* arg)
{
    t_log* logger_hilo = iniciar_logger("logger_hilo_conec_cpu.log","HILO_CPU");
    log_info(logger_hilo, "HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo,"Socket: %i", arg_h->socket);
    //enviar_mensaje("LISTO_PARA_RECIBIR_PEDIDOS",arg_h->socket);
    while(1)
    {
        op_code codigo = recibir_operacion(arg_h->socket);
        log_info(logger_hilo,"op_code: %i", codigo);
        switch (codigo)
        {
        case FETCH_INSTRUCCION:
            uint32_t pid, program_counter;
            recv(arg_h->socket, &pid, sizeof(uint32_t), MSG_WAITALL);
            log_info(logger_hilo,"pid: %i", pid);
            recv(arg_h->socket, &program_counter, sizeof(uint32_t), MSG_WAITALL);
            log_info(logger_hilo,"ip: %i", program_counter);

            sem_wait(&cantidad_de_procesos);
            t_proceso* proceso = buscar_proceso(pid);
            log_info(logger_hilo,"HAY QUE ENVIAR LA INSTRUCCION");
            sem_wait(&mutex_lista_procesos);

            if(program_counter >= proceso->instrucciones->elements_count){
                //Habría que mandarle un mensaje a CPU
                log_info(logger_hilo, "No hay más isntrucciones");
            }
            log_info(logger_hilo,"Envío: %s", list_get(proceso->instrucciones, program_counter));
            enviar_mensaje(list_get(proceso->instrucciones, program_counter), arg_h->socket);
            sem_post(&mutex_lista_procesos);
            sem_post(&cantidad_de_procesos);
            break;
        
        default:
            //break;
        }

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
    sem_init(&mutex_lista_procesos, 0, 1);
    sem_init(&cantidad_de_procesos, 0, 0);
    procesos_en_memoria = list_create();
    t_args_hilo args_conexion_kernel;
    args_conexion_kernel.socket = socket_kernel;
    pthread_t hilo_conexion_kernel;
    log_info(logger, "Declaré el hilo.");
    log_info(logger, "socket: %i", args_conexion_kernel.socket);
    pthread_create(&hilo_conexion_kernel, NULL, &conexion_kernel, (void*)&args_conexion_kernel);
    log_info(logger, "Creé el hilo.");

    pthread_t hilo_conexion_cpu;
    t_args_hilo args_conexion_cpu;
    args_conexion_cpu.socket = socket_cpu;
    pthread_create(&hilo_conexion_cpu, NULL, &conexion_cpu, (void*)&args_conexion_cpu);


    pthread_join(&hilo_conexion_kernel,NULL);
    //pthread_detach(&hilo_conexion_kernel);
    //while(1);
    
}