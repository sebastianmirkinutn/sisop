#include "memoria_de_usuario.h"

extern void* memoria_de_usuario;
extern t_bitarray* frame_bitarray;
extern t_list* procesos_en_memoria;
extern sem_t mutex_lista_procesos;
extern sem_t cantidad_de_procesos;
extern int tam_pagina;
extern int tam_memoria;
extern int cant_frames;
extern t_log* logger;
extern t_list* lista_de_frames;
extern bool algoritmo_de_reemplazo;
int orden_de_frame = 0;
extern int conexion_server_swap;

void conexion_filesystem(void* arg)
{
    t_log* logger_hilo = iniciar_logger("logger_hilo_conec_FS.log","HILO_CPU");
    log_info(logger_hilo, "HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo,"Socket: %i", arg_h->socket_filesystem);
    t_direccion_fisica* direccion;
    //enviar_mensaje("LISTO_PARA_RECIBIR_PEDIDOS",arg_h->socket_cpu);
    op_code codigo;
    uint32_t lectura, a_escribir;
    while(1)
    {
        codigo = recibir_operacion(arg_h->socket_filesystem);
        log_info(logger_hilo,"op_code: %i", codigo);
        switch (codigo)
        {
        case PEDIDO_LECTURA:
            direccion = recibir_direccion(arg_h->socket_filesystem);
            lectura = leer_de_memoria(direccion);
            send(arg_h->socket_filesystem, &lectura, sizeof(uint32_t), 0);
            break;

        case PEDIDO_ESCRITURA:
            direccion = recibir_direccion(arg_h->socket_filesystem);
            recv(arg_h->socket_filesystem, &a_escribir, sizeof(uint32_t), MSG_WAITALL);
            printf("Voy a escribir en memoria\n");
            escribir_en_memoria(direccion, a_escribir);
            break;

        default:
            liberar_conexion(arg_h->socket_filesystem);
            return;
        }

    }
}


#pragma region Operaciones R/W

uint8_t leer_de_memoria(t_direccion_fisica* direccion)
{
    uint8_t leido;
    memcpy(&leido, memoria_de_usuario + (direccion->frame * tam_pagina) + direccion->offset, sizeof(uint8_t));
    if(algoritmo_de_reemplazo)
        actualizar_orden_de_frame(direccion->frame);
    return leido;
}

void escribir_en_memoria(t_direccion_fisica* direccion, uint8_t a_escribir)
{
    if(algoritmo_de_reemplazo)
        actualizar_orden_de_frame(direccion->frame);
    memcpy(memoria_de_usuario + (direccion->frame * tam_pagina) + direccion->offset, &a_escribir, sizeof(uint8_t));
}

#pragma endregion

#pragma region Operaciones generales

void agregar_pagina(uint32_t pid, uint32_t nro_pagina, uint32_t nro_frame)
{
    t_proceso* proceso = buscar_proceso(pid);
    
    if(proceso != NULL)
    {
        printf("Se crea una página\n");
        t_pagina* pagina = crear_pagina(nro_pagina, nro_frame);
        printf("Se creó una página (p = %i - f = %i - s = %i)\n", pagina->pagina, pagina->frame, pagina->posicion_en_swap);
        if(pagina != NULL)
        {
            list_add(proceso->tabla_de_paginas, pagina);
            printf("Se insertó la página\n");
        }
    }
    else
    {
        /*NO EXISTE EL PROCESO*/
    }
}

int32_t obtener_frame(uint32_t pagina_buscada, uint32_t pid)
{
    bool es_la_pagina(void* pagina){
        return ((t_pagina*)pagina)->pagina == pagina_buscada;
    }
    t_proceso* proceso = buscar_proceso(pid);
    if(proceso != NULL)
    {
        printf("Existe el proceso del que se busca el marco\n");
        t_pagina* pagina = list_find(proceso->tabla_de_paginas, es_la_pagina);
        if(pagina != NULL)
        {
            if(pagina->presencia){
                if(algoritmo_de_reemplazo)
                    actualizar_orden_de_frame(pagina->frame);
                return pagina->frame;
            }
            else{
                return -3;  // Page fault, actualizo de swap
            }
        }
        else
            return -2; // Page fault, página no pertenece al proceso
    }
    else
        return -1;  //Page fault, no existe el proceso
}

t_pagina* obtener_pagina_de_frame(t_list* tabla_de_paginas, uint32_t frame){
    bool es_el_frame(void* pagina){
        return ((t_pagina*)pagina)->frame == frame;
    }
    return (t_pagina*)list_find(tabla_de_paginas, es_el_frame);
}

t_pagina* pagina_en_proceso(uint32_t pagina_buscada, uint32_t pid){
    bool es_la_pagina(void* pagina){
        return ((t_pagina*)pagina)->pagina == pagina_buscada;
    }
    t_proceso* proceso = buscar_proceso(pid);
    return (t_pagina*)list_find(proceso->tabla_de_paginas, es_la_pagina);
}

t_proceso* buscar_proceso(uint32_t pid)
{
    bool comparar(void* arg){
        printf("COMPARAR");
        t_proceso* proceso = arg;
        return proceso->pid == pid;
    }
    printf("EMPIEZA BUCAR_PROCESO\n");
    t_proceso* proceso;
    printf("EMPIEZA BUCAR_PROCESO\n");
    printf("hice waitss BUCAR_PROCESO\n");
    //pid_funcion = pid;
    proceso = list_find(procesos_en_memoria, comparar);
    printf("TERMINA BUCAR_PROCESO\n");
    return proceso;
}

void quitar_de_memoria(uint32_t pid){
    // Agregar semaforos
    bool es_mismo_pid(void* proceso){
        return ((t_proceso*)proceso)->pid == pid;
    }
    list_remove_and_destroy_by_condition(procesos_en_memoria, es_mismo_pid, free);
}

#pragma endregion

#pragma region Operaciones con SWAP

void escribir_en_swap(t_pagina* pagina){
    void* contenido_frame = malloc(tam_pagina);
    memcpy(contenido_frame, (memoria_de_usuario + (pagina->frame*tam_pagina)), tam_pagina);
    enviar_operacion(conexion_server_swap, ESCRIBIR_SWAP);
    send(conexion_server_swap, &(pagina->posicion_en_swap), sizeof(uint32_t), 0);
    send(conexion_server_swap, &contenido_frame, tam_pagina, 0);
    free(contenido_frame);
}

void leer_en_swap(t_pagina* pagina){
    uint32_t posicion_en_swap = pagina->posicion_en_swap;
    void* contenido_frame = malloc(tam_pagina);
    enviar_operacion(conexion_server_swap, LEER_SWAP);
    recv(conexion_server_swap, contenido_frame, tam_pagina, MSG_WAITALL);
    memcpy(memoria_de_usuario + (pagina->frame*tam_pagina), contenido_frame, tam_pagina);
    free(contenido_frame);
}

#pragma endregion

#pragma region Logica de remplazo

void actualizar_orden_de_frame(uint32_t frame){
    bool tiene_el_frame(void* _frame_info){
        return (((t_frame_info*)_frame_info)->frame) == frame;
    }
    t_frame_info* frame_info = list_find(lista_de_frames, tiene_el_frame);
    frame_info->orden = orden_de_frame;
    // Posible memory leak
    list_replace_by_condition(lista_de_frames,tiene_el_frame, frame_info);
    orden_de_frame++;
}

void eliminar_de_lista(u_int32_t frame){
    bool tiene_el_frame(void* frame_info){
        return (((t_frame_info*)frame_info)->frame) == frame;
    }
    list_remove_and_destroy_by_condition(lista_de_frames,tiene_el_frame, free);
}


int32_t frame_disponible(){
    int32_t i;
    for(i=0; i < cant_frames; i++){
        if(!bitarray_test_bit(frame_bitarray,i))
            break;
    }
    return (i!=cant_frames * 8) ? i : -1;

}

t_frame_info* elegir_victima(){
    return (t_frame_info*)list_get_minimum(lista_de_frames, menor_que);
}

void eliminar_frame_info(uint32_t frame){
    bool tiene_el_frame(void* frame_info){
        return (((t_frame_info*)frame_info)->frame) == frame;
    }
    // Elimino de la lista de frame_info
    printf("Elimina de la lista a %i", frame);
    list_remove_and_destroy_by_condition(lista_de_frames, tiene_el_frame, free);
    // Libero el bit correspondiente en el bitarray
    bitarray_clean_bit(frame_bitarray, frame);
}

void agregar_frame_info(uint32_t frame, u_int32_t pid){
    // Agrego a la lista de frame_info
    list_add(lista_de_frames, crear_frame_info(frame, pid, orden_de_frame));
    // Aumento el numero de orden del siguiente frame
    orden_de_frame++;
    // Ocupo el bit correspondiente en el bitarray
    bitarray_set_bit(frame_bitarray, frame);
}

void actualizar_bitarray(t_list* tabla_de_paginas){
    t_pagina* pagina;
    for(int i=0; i < list_size(tabla_de_paginas);i++){
        pagina = list_get(tabla_de_paginas, i);
        if(pagina->presencia)
            bitarray_clean_bit(frame_bitarray, pagina->frame);
        
    }
}

void remover_proceso_de_lista_de_orden(u_int32_t pid){
    bool es_del_proceso(void* frame_info){
        return (((t_frame_info*)frame_info)->pid) == pid;
    }
    list_remove_and_destroy_by_condition(lista_de_frames, es_del_proceso, free);
}


uint32_t reemplazo_de_frame(uint32_t pid, uint32_t pagina_reemplazante, bool leer_swap, t_log* logger_hilo){
    t_frame_info* frame_victima_info = elegir_victima();
    log_info(logger, "Se eligio la victima %u", frame_victima_info->frame);
    sem_wait(&cantidad_de_procesos);
    sem_wait(&mutex_lista_procesos);
    t_proceso* proceso_victima = buscar_proceso(frame_victima_info->pid);
    t_pagina* pagina_victima = obtener_pagina_de_frame(proceso_victima->tabla_de_paginas, frame_victima_info->frame);
    if(pagina_victima->modificado){
        escribir_en_swap(pagina_victima);
        log_info(logger_hilo, "SWAP OUT - PID: %u - Marco: %u - Page Out: %u-%u", proceso_victima->pid, frame_victima_info->frame, proceso_victima->pid, pagina_victima->pagina);
    }
    uint32_t frame = frame_victima_info->frame;
    sem_post(&mutex_lista_procesos);
    sem_post(&cantidad_de_procesos);
    if(leer_swap){
        sem_wait(&cantidad_de_procesos);
        sem_wait(&mutex_lista_procesos);
        t_pagina* pagina = pagina_en_proceso(pagina_reemplazante, pid);
        pagina->frame = frame;
        pagina->presencia = 1;
        pagina->modificado = 0;
        leer_en_swap(pagina);
        log_info(logger_hilo, "SWAP IN - PID %u - Marco: %u - Page In: %u-%u", pid, frame, pid, pagina->pagina);
        log_info(logger_hilo, "REEMPLAZO - Marco: %u - Page Out: %u-%u - Page In: %u-%u", frame, proceso_victima->pid,pagina_victima->pagina,pid,pagina_reemplazante);
        sem_post(&mutex_lista_procesos);
        sem_post(&cantidad_de_procesos);
        agregar_frame_info(frame, pid);
    }
    
    eliminar_frame_info(frame_victima_info->frame);
    return frame;
}

#pragma endregion