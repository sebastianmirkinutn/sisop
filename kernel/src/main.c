#include "../include/main.h"

sem_t grado_de_multiprogramacion;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t procesos_en_new;


t_queue *cola_ready;
t_queue *cola_blocked;
t_queue *cola_exit;
t_queue *cola_new;

void planificador_largo_plazo(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    t_queue* cola_ready_h = &cola_ready;

    while(1)
    {
        sem_wait(&procesos_en_new);
        sem_wait(&grado_de_multiprogramacion);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_new);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);
        t_pcb* pcb = queue_pop(cola_new);
        sem_post(&mutex_cola_new);
        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready_h, pcb);
        sem_post(&mutex_cola_ready);
        log_info(logger_hilo, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", pcb->pid);

        op_code operacion = INICIAR_PROCESO;
        send(arg_h->socket, &operacion, sizeof(op_code), 0);
        send(arg_h->socket, &pcb->pid, sizeof(int), 0);
        enviar_mensaje(pcb->archivo_de_pseudocodigo, arg_h->socket);     



        liberar_pcb(pcb);
    }
}

void planificador_corto_plazo(/*ALGORITMO*/)
{
    while(1)
    {
        //t_registros* contexto = recibir_contexto_de_ejecucion();

    }
}

int main(int argc, char* argv[]){
   
    t_log* logger = iniciar_logger("log_kernel","Kernel");
    t_config* config = iniciar_config("./cfg/kernel.config");
    
    char* ip_cpu = config_get_string_value(config, "IP_CPU");
	char* puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    char* puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    char* ip_filesystem = config_get_string_value(config, "IP_FILESYSTEM");
	char* puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");
    char* grado_max_de_multiprogramacion = config_get_string_value(config, "GRADO_MULTIPROGRAMACION_INI");

    int conexion_cpu_dispatch = crear_conexion(logger, ip_cpu, puerto_cpu_dispatch);
    int conexion_cpu_interrupt = crear_conexion(logger, ip_cpu, puerto_cpu_interrupt);
    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);
    int conexion_filesystem = crear_conexion(logger, ip_filesystem, puerto_filesystem);

    sem_init(&mutex_cola_new, 0, 1);
    sem_init(&mutex_cola_ready, 0, 1);
    sem_init(&procesos_en_new, 0, 0);
    sem_init(&grado_de_multiprogramacion, 0, atoi(grado_max_de_multiprogramacion));
    //log_info(logger,"GDMP: %i",atoi(grado_max_de_multiprogramacion));

    //log_info(logger, " ANTES DE QUEUE_CREATE():%i",cola_new);
    cola_new = queue_create();
    //log_info(logger, "QUEUE_CREATE():%i",cola_new);
	cola_ready = queue_create();
	cola_blocked = queue_create();
	cola_exit = queue_create();

    t_args_hilo args_conexion_memoria;
    args_conexion_memoria.socket = conexion_memoria;
    
    pthread_t hilo_planificador_de_largo_plazo;
    //printf("Declaré el hilo\n");
    pthread_create(&hilo_planificador_de_largo_plazo, NULL, &planificador_largo_plazo, (void*)&args_conexion_memoria);
    //printf("Creé el hilo\n");
    pthread_detach(&hilo_planificador_de_largo_plazo);
    //printf("Desvinculé el hilo\n");
    //log_info(logger,"Cola new %i", cola_new);

    while(1){
        t_mensaje mensaje;
        char* leido = readline("> ");
        add_history(leido);
        //log_info(logger,leido);
        char* token = strtok(leido, " ");
        char* c_argv[4]; //Va a haber como máximo 4 tokens.
        int i = 0;
        c_argv[i] = strdup(token);
        token = strtok(NULL, " ");


        while(token != NULL && i < 4)
        {
            i++;
            if(i < 4){
                c_argv[i] = strdup(token);
            }
            token = strtok(NULL, " ");
        }
        printf(" Parametros %i",i);

        if(!strcmp(c_argv[0], "INICIAR_PROCESO")){
            /*INICIAR_PROCESO [PATH] [SIZE] [PRIORIDAD]*/
            if (i < 3)
            {
                log_warning(logger, "Se pasaron parámetros de menos");
            }
            else if (i > 3)
            {
                log_warning(logger, "Se pasaron parámetros de más");
            }
            
            //mensaje.size_mensaje = strlen(c_argv[0]) + 1;
            //enviar_mensaje(mensaje.mensaje ,conexion_cpu_dispatch);
            //mensaje.mensaje = c_argv[0];
            //mensaje.mensaje = c_argv[1];
            //mensaje.size_mensaje = strlen(c_argv[1]) + 1;
            //enviar_mensaje(mensaje.mensaje ,conexion_cpu_dispatch);

            //mensaje.mensaje = c_argv[2];
            //mensaje.size_mensaje = strlen(c_argv[2]) + 1;
            //enviar_mensaje(mensaje.mensaje ,conexion_cpu_dispatch);

            //mensaje.mensaje = c_argv[3];
            //mensaje.size_mensaje = strlen(c_argv[3]) + 1;

            //enviar_mensaje(mensaje.mensaje ,conexion_cpu_dispatch);

            
            t_pcb* pcb = crear_pcb(atoi(c_argv[3]), c_argv[1]);
            sem_wait(&mutex_cola_new);
            //log_info(logger,"Hice wait");
            queue_push(cola_new, pcb);
            //log_info(logger,"Hice push");
            sem_post(&mutex_cola_new);
            sem_post(&procesos_en_new);

            log_info(logger, "Se crea el proceso %i en NEW", pcb->pid);
        }
        else if(!strcmp(c_argv[0], "FINALIZAR_PROCESO")){

        }
        else if(!strcmp(c_argv[0], "DETENER_PLANIFICACION")){

        }
        else if(!strcmp(c_argv[0], "INICIAR_PLANIFICACION")){

        }
        else if(!strcmp(c_argv[0], "MULTIPROGRAMACION")){

        }
        else if(!strcmp(c_argv[0], "PROCESO_ESTADO")){

        }
        else{
            log_warning(logger, "La función %s no existe.", c_argv[0]);
        }
    }

    //enviar_mensaje("Mensaje", conexion_cpu);

    log_destroy(logger);
    config_destroy(config);
    liberar_conexion(conexion_cpu_dispatch);
    
}

