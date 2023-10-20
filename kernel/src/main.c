#include "../include/main.h"

sem_t grado_de_multiprogramacion;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t procesos_en_new;
sem_t procesos_en_ready;
sem_t planificacion_largo_plazo;
sem_t planificacion_corto_plazo;


t_queue *cola_ready;
t_queue *cola_blocked;
t_queue *cola_exit;
t_queue *cola_new;

t_pcb* execute;

char* de_t_motivo_a_string(t_motivo_desalojo motivo)
{
    switch (motivo)
    {
    case SUCCESS:
        return "SUCCESS";
        break;

    case INVALID_RESOURCE:
        return "INVALID_RESOURCE";
        break;

    case INVALID_WRITE:
        return "INVALID_WRITE";
        break;

    case CLOCK_INTERRUPT:
        return "CLOCK_INTERRUPT";
        break;
    
    default:
        return "OTRO";
        break;
    }
}

void evaluar_motivo_desalojo(t_motivo_desalojo motivo)
{
    switch (motivo)
    {
    case SUCCESS:
        /* AGREGO A COLA_EXIT */
        break;

    case CLOCK_INTERRUPT:
        sem_wait(&mutex_cola_ready);
        execute->estado = READY;
        queue_push(cola_ready, execute);
        sem_post(&mutex_cola_ready);
        sem_post(&procesos_en_ready);
        break;
    
    default:
        break;
    }
}

void planificador_largo_plazo(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;

    while(1)
    {
        sem_wait(&planificacion_largo_plazo);
        sem_wait(&procesos_en_new);
        sem_wait(&grado_de_multiprogramacion);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_new);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);
        t_pcb* pcb = queue_pop(cola_new);
        sem_post(&mutex_cola_new);
        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, pcb);
        sem_post(&mutex_cola_ready);
        log_info(logger_hilo, "PID:%i - Estado:%i", pcb->pid, pcb->estado);
        log_info(logger_hilo, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", pcb->pid);

        op_code operacion = INICIAR_PROCESO;
        send(arg_h->socket_memoria, &operacion, sizeof(op_code), 0);
        send(arg_h->socket_memoria, &(pcb->pid), sizeof(int), 0);
        enviar_mensaje(pcb->archivo_de_pseudocodigo, arg_h->socket_memoria);     
        sem_post(&procesos_en_ready);
        sem_post(&planificacion_largo_plazo);
    }
}

sem_t start_interrupts;
sem_t mutex_flag_p_finished;
int p_finished;

void clock_interrupt(void* arg)
{
    t_log* logger_hilo_ci = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo_ci, "Empieza clock_interrupt");
    op_code operacion = INTERRUPT;

    while(1)
    {
        sem_wait(&start_interrupts);
        sleep(2);
        sem_wait(&mutex_flag_p_finished);
        if (!p_finished) send(arg_h->socket_interrupt, &operacion, sizeof(op_code), 0);
        sem_post(&mutex_flag_p_finished);

    }
}

void planificador_rr(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo, "Empieza el planificador fifo");
    pthread_t generador_de_interrupciones;
    sem_init(&start_interrupts, 0, 0);
    sem_init(&mutex_flag_p_finished, 0, 1);
    pthread_create(&generador_de_interrupciones, NULL, &clock_interrupt, arg);
    pthread_detach(&generador_de_interrupciones);
    while(1)
    {
        sem_wait(&planificacion_corto_plazo);
        sem_wait(&procesos_en_ready);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_ready);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);

        execute = queue_pop(cola_ready);
        sem_post(&mutex_cola_ready);
        execute->estado = EXEC;

        log_info(logger_hilo, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", execute->pid);
  
        send(arg_h->socket_dispatch, &(execute->pid), sizeof(uint32_t), 0);
        log_info(logger_hilo, "Envié %i a %i", execute->pid, arg_h->socket_dispatch);
        send(arg_h->socket_dispatch, &(execute->contexto->AX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->BX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->CX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->DX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->PC), sizeof(uint32_t), 0);


        log_info(logger_hilo, "Envié todo");
        sem_wait(&mutex_flag_p_finished);
        p_finished = 0;
        sem_post(&mutex_flag_p_finished);
        sem_post(&start_interrupts);
        log_info(logger_hilo, "Pasé el sem_post");
        t_motivo_desalojo motivo = recibir_desalojo(arg_h->socket_dispatch);
        recv(arg_h->socket_dispatch, &(execute->contexto->BX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->AX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->CX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->DX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->PC), sizeof(uint32_t), MSG_WAITALL);
        sem_wait(&mutex_flag_p_finished);
        p_finished = 1;
        sem_post(&mutex_flag_p_finished);
        log_info(logger_hilo, "Recibí el desalojo y el contexto");
        int* a = execute->pid;
        log_info(logger_hilo, "Fin de proceso %i motivo %s", execute->pid, de_t_motivo_a_string(motivo));
        evaluar_motivo_desalojo(motivo);
        sem_post(&planificacion_corto_plazo);
    }
}

void planificador_fifo(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo, "Empieza el planificador fifo");
    while(1)
    {
        sem_wait(&planificacion_corto_plazo);
        sem_wait(&procesos_en_ready);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_ready);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);

        execute = queue_pop(cola_ready);
        sem_post(&mutex_cola_ready);
        execute->estado = EXEC;

        log_info(logger_hilo, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", execute->pid);
  
        send(arg_h->socket_dispatch, &(execute->pid), sizeof(uint32_t), 0);
        log_info(logger_hilo, "Envié %i a %i", execute->pid, arg_h->socket_dispatch);
        send(arg_h->socket_dispatch, &(execute->contexto->AX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->BX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->CX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->DX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->PC), sizeof(uint32_t), 0);

        t_motivo_desalojo motivo = recibir_desalojo(arg_h->socket_dispatch);
        recv(arg_h->socket_dispatch, &(execute->contexto->BX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->AX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->CX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->DX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->PC), sizeof(uint32_t), MSG_WAITALL);
        char* motivo_de_finalizacion = de_t_motivo_a_string(motivo);
        log_info(logger_hilo, "Fin de proceso %i motivo %s", execute->pid, motivo_de_finalizacion);
        evaluar_motivo_desalojo(motivo);
        sem_post(&planificacion_corto_plazo);
    }
}

void ordenar_colas_segun_prioridad(t_queue* queue)
{
    bool comparar_prioridad(void* arg1, void* arg2)
    {
        t_pcb* pcb1 = (t_pcb*) arg1;
        t_pcb* pcb2 = (t_pcb*) arg2;
        return (pcb1->prioridad < pcb2->prioridad);
    }
    list_sort(queue->elements, (bool*)&comparar_prioridad);
}

void planificador_prioridades(void* arg)
{
    t_log* logger_hilo = iniciar_logger("log_plani.log","HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo, "Empieza el planificador por prioridades");
    while(1)
    {
        sem_wait(&planificacion_corto_plazo);
        sem_wait(&procesos_en_ready);
        log_info(logger_hilo,"Hice wait del gdmp");
        sem_wait(&mutex_cola_ready);
        log_info(logger_hilo,"Hice wait de la cola de new: %i",cola_new);

        ordenar_colas_segun_prioridad(cola_ready);
        execute = queue_pop(cola_ready);
        sem_post(&mutex_cola_ready);
        execute->estado = EXEC;

        log_info(logger_hilo, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", execute->pid);

        send(arg_h->socket_dispatch, &(execute->pid), sizeof(uint32_t), 0);
        log_info(logger_hilo, "Envié %i a %i", execute->pid, arg_h->socket_dispatch);
        send(arg_h->socket_dispatch, &(execute->contexto->AX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->BX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->CX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->DX), sizeof(uint32_t), 0);
        send(arg_h->socket_dispatch, &(execute->contexto->PC), sizeof(uint32_t), 0);

        t_motivo_desalojo motivo = recibir_desalojo(arg_h->socket_dispatch);
        recv(arg_h->socket_dispatch, &(execute->contexto->BX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->AX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->CX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->DX), sizeof(uint32_t), MSG_WAITALL);
        recv(arg_h->socket_dispatch, &(execute->contexto->PC), sizeof(uint32_t), MSG_WAITALL);
        char* motivo_de_finalizacion = de_t_motivo_a_string(motivo);
        log_info(logger_hilo, "Fin de proceso %i motivo %s", execute->pid, motivo_de_finalizacion);
        log_info(logger_hilo, "AX:%i - BX:%i - CX:%i - DX:%i - PC:%i", execute->contexto->AX, execute->contexto->BX, execute->contexto->CX, execute->contexto->DX, execute->contexto->PC);

        evaluar_motivo_desalojo(motivo);

        sem_post(&planificacion_corto_plazo);
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
    char* algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

    int conexion_cpu_dispatch = crear_conexion(logger, ip_cpu, puerto_cpu_dispatch);
    int conexion_cpu_interrupt = crear_conexion(logger, ip_cpu, puerto_cpu_interrupt);
    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);
    int conexion_filesystem = crear_conexion(logger, ip_filesystem, puerto_filesystem);

    sem_init(&mutex_cola_new, 0, 1);
    sem_init(&mutex_cola_ready, 0, 1);
    sem_init(&procesos_en_new, 0, 0);
    sem_init(&procesos_en_ready, 0, 0);
    sem_init(&grado_de_multiprogramacion, 0, atoi(grado_max_de_multiprogramacion));
    sem_init(&planificacion_largo_plazo, 0, 0);
    sem_init(&planificacion_corto_plazo, 0, 0);

    cola_new = queue_create();
	cola_ready = queue_create();
	cola_blocked = queue_create();
	cola_exit = queue_create();

    t_args_hilo args_hilo;
    args_hilo.socket_dispatch = conexion_cpu_dispatch;
    args_hilo.socket_interrupt = conexion_cpu_interrupt;
    args_hilo.socket_memoria = conexion_memoria;
    
    pthread_t hilo_planificador_de_largo_plazo;
    pthread_create(&hilo_planificador_de_largo_plazo, NULL, &planificador_largo_plazo, (void*)&args_hilo);
    pthread_detach(&hilo_planificador_de_largo_plazo);

    pthread_t hilo_planificador_de_corto_plazo;

    void (*planificador_corto_plazo) (void*) = NULL;
    if(!strcmp(algoritmo_planificacion, "FIFO"))
    {
        planificador_corto_plazo = &planificador_fifo;
    }
    else if(!strcmp(algoritmo_planificacion, "PRIORIDADES"))
    {
        planificador_corto_plazo = &planificador_prioridades;
    }
    else if(!strcmp(algoritmo_planificacion, "RR"))
    {
        planificador_corto_plazo = &planificador_rr;
    }

    pthread_create(&hilo_planificador_de_corto_plazo, NULL, planificador_corto_plazo, (void*)&args_hilo);
    pthread_detach(&hilo_planificador_de_corto_plazo);
    log_info(logger,"Creé los hilos");

    int sem_value_lp, sem_value_cp;

    while(1){
        t_mensaje mensaje;
        fflush(stdin);
        char* leido = readline("> ");
        add_history(leido);
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

        if(!strcmp(c_argv[0], "INICIAR_PROCESO"))
        {
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
        else if(!strcmp(c_argv[0], "FINALIZAR_PROCESO"))
        {

        }
        else if(!strcmp(c_argv[0], "DETENER_PLANIFICACION"))
        {
            //sem_getvalue(&planificacion_largo_plazo,&sem_value_lp);
            //sem_getvalue(&planificacion_corto_plazo,&sem_value_cp);
            //if(!sem_value_lp) sem_wait(&planificacion_largo_plazo);
            //if(!sem_value_cp) sem_wait(&planificacion_corto_plazo);
            log_info(logger, "Detener");
            sem_wait(&planificacion_largo_plazo);
            sem_wait(&planificacion_corto_plazo);
            log_info(logger, "Se detuvo");

        }
        else if(!strcmp(c_argv[0], "INICIAR_PLANIFICACION"))
        {
            //sem_getvalue(&planificacion_largo_plazo,&sem_value_lp);
            //sem_getvalue(&planificacion_corto_plazo,&sem_value_cp);
            //if(!sem_value_lp) sem_post(&planificacion_largo_plazo);
            //if(!sem_value_cp) sem_post(&planificacion_corto_plazo);
            log_info(logger, "Iniciar");
            sem_post(&planificacion_largo_plazo);
            sem_post(&planificacion_corto_plazo);
            log_info(logger, "Se inició");
        
        }
        else if(!strcmp(c_argv[0], "MULTIPROGRAMACION"))
        {

        }
        else if(!strcmp(c_argv[0], "PROCESO_ESTADO"))
        {   

        }
        else if(!strcmp(c_argv[0], "INTERRUPT"))
        {
            op_code operacion = INTERRUPT;
            send(conexion_cpu_interrupt, &operacion, sizeof(op_code), 0);
        }
        else
        {
            log_warning(logger, "La función %s no existe.", c_argv[0]);
        }
    }

    log_destroy(logger);
    config_destroy(config);
    liberar_conexion(conexion_cpu_dispatch);
    
}
