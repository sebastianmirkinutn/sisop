#include "swap.h"

extern FILE* bloques;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern t_list* lista_de_bloques_swap;
uint32_t orden_de_bloque;
void* buffer_swap;
t_bitarray* swap_bitarray;

void conexion_memoria(void* arg)
{
    t_log* logger_hilo = iniciar_logger("logger_hilo.log","HILO");
    log_info(logger_hilo, "HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    log_info(logger_hilo, "Socket memoria: %i", arg_h->socket_memoria);
    lista_de_bloques_swap = list_create();
    buffer_swap = malloc(tam_bloque);
    uint32_t pid;
    uint32_t cantidad_de_bloques;
    uint32_t bloque;
    while(1)
    {
        op_code codigo = recibir_operacion(arg_h->socket_memoria);
        switch (codigo)
        {
            case RESERVAR_SWAP:
                recv(arg_h->socket_memoria, &cantidad_de_bloques, sizeof(uint32_t), MSG_WAITALL);
                log_info(logger_hilo, "Pide reservar %i bloques", cantidad_de_bloques);
                recv(arg_h->socket_memoria, &pid, sizeof(uint32_t), MSG_WAITALL);
                log_info(logger_hilo, "el proceso %i", pid);
                reservar_bloques_swap(cantidad_de_bloques, pid, logger_hilo);
                log_info(logger_hilo, "Se reservaron.");
                enviar_respuesta(arg_h->socket_memoria,OK);
                break;
	        case ESCRIBIR_SWAP:
                recv(arg_h->socket_memoria, &pid, sizeof(uint32_t), MSG_WAITALL);
                if(proceso_esta_reservado(pid)){
                    enviar_respuesta(arg_h->socket_memoria, OK);
                    recv(arg_h->socket_memoria, &bloque, sizeof(uint32_t), MSG_WAITALL);
                    recv(arg_h->socket_memoria, &buffer_swap, (size_t)tam_bloque, MSG_WAITALL);
                    if(bloque==0){
                        bloque = escribir_nuevo_bloque_swap(buffer_swap, pid, logger_hilo);
                        send(arg_h->socket_memoria, &bloque, sizeof(uint32_t), 0);
                    }
                    else{
                        escribir_bloque_swap(bloque, buffer_swap);
                        enviar_respuesta(arg_h->socket_memoria, OK);
                    }
                }
                break;
	        case LEER_SWAP:
                recv(arg_h->socket_memoria, &bloque, sizeof(uint32_t), MSG_WAITALL);
                if(bloque != 0){
                    leer_bloque_swap(bloque);
                    send(arg_h->socket_memoria, &buffer_swap, (size_t)tam_bloque, MSG_WAITALL);
                }

                break;
	        case ELIMINAR_SWAP:
                recv(arg_h->socket_memoria, &pid, sizeof(uint32_t), MSG_WAITALL);
                eliminar_en_swap(pid);
                enviar_respuesta(arg_h->socket_memoria, OK);
                break;
        default:
            //list_destroy_and_destroy_elements(lista_de_bloques_swap, free);
            //liberar_conexion(arg_h->socket_memoria);
            return;
        }

    }

}

void reservar_bloques_swap(uint32_t cantidad, uint32_t pid, t_log* logger_hilo){
    uint32_t bloque_aux;
    int i;
    log_info(logger_hilo, "entra");
    for(i = 0; i < cantidad; i++){
        log_info(logger_hilo,"reservo bloque %i", i);
        bloque_aux = reservar_bloque_swap(pid, logger_hilo);
        log_info(logger_hilo, "Se reservo el bloque %d para el proceso %d en SWAP", bloque_aux, pid);
    }
}

uint32_t reservar_bloque_swap(uint32_t pid, t_log* logger_hilo){
    uint32_t bloque = elegir_bloque_swap(logger_hilo);
    bitarray_set_bit(swap_bitarray, (off_t)bloque);
    char contenido = '\0';
    for(int i = 0; i < tam_bloque; i++){
        fseek(bloques, bloque + i, SEEK_SET);
        fwrite(&contenido, sizeof(char), 1, bloques);
    }
    t_bloque_swap_info* bloque_info = malloc(sizeof(t_bloque_swap_info));
    bloque_info->bloque = bloque;
    bloque_info->pid = pid;
    list_add(lista_de_bloques_swap, bloque_info);
    return bloque;
}

bool proceso_esta_reservado(uint32_t pid){
    bool es_el_pid(void* bloque_swap_info){
        return ((t_bloque_swap_info*)bloque_swap_info)->pid == pid;
    }
    return list_any_satisfy(lista_de_bloques_swap, es_el_pid);
}

uint32_t elegir_bloque_swap(t_log* logger_hilo){
    int i;
    t_bloque_swap_info* bloque_info;
    for(i=0; i < swap_bitarray->size * 8; i++){
        if(!bitarray_test_bit(swap_bitarray,i))
            break;
    }
    if(i >= swap_bitarray->size * 8){
        bloque_info = list_get(lista_de_bloques_swap, 0);
        i = bloque_info->bloque;
        list_remove_and_destroy_element(lista_de_bloques_swap, 0, free);
    }
    return i;
}

void leer_bloque_swap(uint32_t bloque){
    fseek(bloques, bloque * tam_bloque, SEEK_SET);
    fread(&buffer_swap, (size_t)tam_bloque, 1, bloques);
}

void escribir_bloque_swap(uint32_t bloque, void* contenido){
    fseek(bloques, bloque * tam_bloque, SEEK_SET);
    fwrite(&buffer_swap, (size_t)tam_bloque, 1, bloques);
}

uint32_t escribir_nuevo_bloque_swap(void* contenido, uint32_t pid, t_log* logger_hilo){
    uint32_t bloque = elegir_bloque_swap(logger_hilo);
    fseek(bloques, bloque, SEEK_SET);
    fwrite(&contenido, (size_t)tam_bloque, 1, bloques);
    t_bloque_swap_info* bloque_info = malloc(sizeof(t_bloque_swap_info));
    bloque_info->bloque = bloque;
    bloque_info->pid = pid;
    list_add(lista_de_bloques_swap, bloque_info);
    return bloque;
}

void eliminar_en_swap(uint32_t pid){
    t_bloque_swap_info* bloque_info;
    for(int i=0; i< list_size(lista_de_bloques_swap); i++){
        bloque_info = list_get(lista_de_bloques_swap, i);
        if(bloque_info->pid == pid){
            bitarray_clean_bit(swap_bitarray, bloque_info->bloque);
            list_remove_and_destroy_element(lista_de_bloques_swap, i, free);
        }
    }
}