#include "interrupciones.h"

extern t_registros* registros;

extern sem_t mutex_flag_interrupciones;

extern int flag_interrupciones;
extern int execute;

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
        enviar_operacion(socket_kernel_dispatch, DESALOJO);
        enviar_motivo_desalojo(socket_kernel_dispatch, CLOCK_INTERRUPT);
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