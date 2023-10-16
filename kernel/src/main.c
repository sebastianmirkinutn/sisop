#include "../include/main.h"

sem_t grado_de_multiprogramacion;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t procesos_en_new;
sem_t procesos_en_ready;


t_queue *cola_ready;
t_queue *cola_blocked;
t_queue *cola_exit;
t_queue *cola_new;

t_pcb* execute;

void planificador_largo_plazo(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;

    while(1)
    {
        sem_wait(&procesos_en_new);
        sem_wait(&grado_de_multiprogramacion);
        //log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_new);
        //log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);
        //t_pcb* pcb = queue_pop(cola_new);
        sem_post(&mutex_cola_new);
        sem_wait(&mutex_cola_ready);
        //queue_push(cola_ready, pcb);
        sem_post(&mutex_cola_ready);
        //log_info(logger_hilo, "PID:%i - Estado:%i", pcb->pid, pcb->estado);
        //log_info(logger_hilo, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", pcb->pid);

        //op_code operacion = INICIAR_PROCESO;
        //send(arg_h->socket, &operacion, sizeof(op_code), 0);
        //send(arg_h->socket, &pcb->pid, sizeof(int), 0);
        //enviar_mensaje(pcb->archivo_de_pseudocodigo, arg_h->socket);     
        sem_post(&procesos_en_ready);



        //liberar_pcb(pcb);
    }
}


void planificador_corto_plazo(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo, "Empieza el planificador fifo");
    while(1)
    {
        sem_wait(&procesos_en_ready);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_ready);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);

        //Hardcodeo para probar
        //t_pcb* pcb_prueba = crear_pcb(1,"");
        //queue_push(cola_ready,pcb_prueba);

        //t_pcb* pcb = queue_pop(cola_ready);
        sem_post(&mutex_cola_ready);
        //pcb->estado = EXEC;
        //sem_wait(&mutex_cola_ready);
        //queue_push(cola_ready, pcb);
        //sem_post(&mutex_cola_ready);
        //log_info(logger_hilo, "PID:%i - Estado:%i", pcb->pid, pcb->estado);
        //log_info(logger_hilo, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", pcb->pid);
        enviar_mensaje("PRUEBA_HILO", arg_h->socket);
        send(arg_h->socket, &(execute->pid), sizeof(uint32_t), 0);
        log_info(logger_hilo, "Envié %i a %i", execute->pid, arg_h->socket);
        send(arg_h->socket, &(execute->contexto->AX), sizeof(uint32_t), 0);
        send(arg_h->socket, &(execute->contexto->BX), sizeof(uint32_t), 0);
        send(arg_h->socket, &(execute->contexto->CX), sizeof(uint32_t), 0);
        send(arg_h->socket, &(execute->contexto->DX), sizeof(uint32_t), 0);
        send(arg_h->socket, &(execute->program_counter), sizeof(uint32_t), 0);

        //pcb->contexto = recibir_contexto_de_ejecucion(arg_h->socket);
        //t_motivo_desalojo motivo = recibir_desalojo(arg_h->socket);
        //log_info(logger_hilo, "Fin de proceso %i motivo %i", pcb->pid, motivo);
        //sem_wait(&mutex_cola_ready);
        //queue_push(cola_ready, pcb);
        //sem_post(&mutex_cola_ready);
        //liberar_pcb(pcb);
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
    sem_init(&procesos_en_ready, 0, 0);
    sem_init(&grado_de_multiprogramacion, 0, atoi(grado_max_de_multiprogramacion));

    cola_new = queue_create();
	cola_ready = queue_create();
	cola_blocked = queue_create();
	cola_exit = queue_create();

    execute = crear_pcb(1, "");

    //enviar_mensaje("PRUEBA_HILO", conexion_cpu_dispatch);
    log_info(logger, "Socket cpu dispatch:%i",conexion_cpu_dispatch);

    t_args_hilo args_conexion_memoria;
    args_conexion_memoria.socket = conexion_memoria;

    t_args_hilo args_conexion_cpu;
    args_conexion_cpu.socket = conexion_cpu_dispatch;
    
    pthread_t hilo_planificador_de_largo_plazo;
    pthread_create(&hilo_planificador_de_largo_plazo, NULL, &planificador_largo_plazo, (void*)&args_conexion_memoria);
    pthread_detach(&hilo_planificador_de_largo_plazo);

    pthread_t hilo_planificador_de_corto_plazo;
    pthread_create(&hilo_planificador_de_corto_plazo, NULL, &planificador_corto_plazo, (void*)&args_conexion_cpu);
    pthread_detach(&hilo_planificador_de_corto_plazo);
    log_info(logger,"Creé los hilos");


    //t_pcb* pcb_prueba = crear_pcb(1, "");
    execute->contexto->AX = 1;
    execute->contexto->BX = 2;
    execute->contexto->CX = 3;
    execute->contexto->DX = 4;
    execute->contexto->PC = 0;
//
    //t_registros* a_serializar = pcb_prueba->contexto;
    //void* serializado = serializar_contexto(a_serializar);
    //t_registros* deserializados = deserializar_contexto(serializado);
    //log_info(logger, "Serializado: %i",serializado);
    //log_info(logger, "AX:%i - BX:%i - CX:%i - DX:%i - PC:%i", deserializados->AX, deserializados->BX, deserializados->CX, deserializados->DX, deserializados->PC);
//
//
    //printf("Voy a enviar el contexto\n");
    //enviar_mensaje((char*) serializado, conexion_cpu_dispatch);
    //getchar();
    //enviar_contexto(pcb_prueba->contexto,conexion_cpu_dispatch);
    //printf("Envié el contexto\n");
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
            else{
                t_pcb* pcb = crear_pcb(atoi(c_argv[3]), c_argv[1]);
                sem_wait(&mutex_cola_new);
                //log_info(logger,"Hice wait");
                queue_push(cola_new, pcb);
                //log_info(logger,"Hice push");
                sem_post(&mutex_cola_new);
                sem_post(&procesos_en_new);

                log_info(logger, "Se crea el proceso %i en NEW", pcb->pid);
            }
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

    log_destroy(logger);
    config_destroy(config);
    liberar_conexion(conexion_cpu_dispatch);
    
}

/*
void planificador_corto_plazo(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;

    t_pcb* pcb; 
    while(1)
    {   
        char* algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

        if(!strcmp(algoritmo, "FIFO")) {
			pcb = algoritmo_fifo();
		} else if (!strcmp(algoritmo, "RR")){
			pcb = algoritmo_rr();
		} else if (!strcmp(algoritmo, "Prioridades")){
			pcb = algoritmo_prioridades();
		} else {
            log_error(logger_hilo, "El algoritmo de planificacion ingresado no existe\n");
        }

        
        log_info(logger_hilo, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", pcb->pid);
        pcb->estado = EXEC; 
        execute = pcb;
        send(arg_h->socket, pcb->pid, sizeof(uint32_t), 0);
        enviar_contexto(pcb->contexto, arg_h->socket);
        log_info(logger_hilo, "mandé el contexto");

        pcb->contexto = recibir_contexto_de_ejecucion(arg_h->socket);
        t_motivo_desalojo motivo = recibir_desalojo(arg_h->socket);
        log_info(logger_hilo, "Fin de proceso %i motivo %i", pcb->pid, motivo);
        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, pcb);
        sem_post(&mutex_cola_ready);
        liberar_pcb(pcb);
    }
}


t_pcb* algoritmo_fifo(){

    sem_wait(&procesos_en_ready);
        sem_wait(&mutex_cola_ready);
        t_pcb* pcb = queue_pop(cola_ready);
        sem_post(&mutex_cola_ready);

}

*/