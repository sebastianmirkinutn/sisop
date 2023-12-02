#include "swap.h"

extern FILE* bloques;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern t_bitarray* swap_bitarray;
extern t_list* lista_de_bloques_swap;
uint32_t orden_de_bloque;


void conexion_memoria(void* arg)
{
    t_log* logger_hilo = iniciar_logger("logger_hilo.log","HILO");
    log_info(logger_hilo, "HILO");
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    swap_bitarray = bitarray_create(swap_bitarray, cant_bloques_swap);
    lista_de_bloques_swap = queue_create();
    
    while(1)
    {
        op_code codigo = recibir_operacion(arg_h->socket_memoria);
        log_info(logger_hilo,"op_code: %i", codigo);
        switch (codigo)
        {
            case RESERVAR_SWAP:
                uint32_t cantidad_de_bloques;
                uint32_t pid;
                recv(arg_h->socket_memoria, &cantidad_de_bloques, sizeof(uint32_t), MSG_WAITALL);
                recv(arg_h->socket_memoria, &pid, sizeof(uint32_t), MSG_WAITALL);
                reservar_bloques_swap(cantidad_de_bloques, pid);
                enviar_respuesta(arg_h,OK);
                break;
	        case ESCRIBIR_SWAP:
                uint32_t pid;
                uint32_t bloque;
                void* contenido = malloc(tam_bloque);
                recv(arg_h->socket_memoria, &pid, sizeof(uint32_t), MSG_WAITALL);
                if(proceso_esta_reservado(pid)){
                    enviar_respuesta(arg_h->socket_memoria, OK);
                    recv(arg_h->socket_memoria, &bloque, sizeof(uint32_t), MSG_WAITALL);
                    recv(arg_h->socket_memoria, &contenido, (size_t)tam_bloque, MSG_WAITALL);
                    if(bloque==0){
                        bloque = escribir_nuevo_bloque_swap(contenido);
                        send(arg_h->socket_memoria, &bloque, sizeof(uint32_t), NULL);
                    }
                    else{
                        escribir_bloque_swap(bloque, contenido);
                        enviar_respuesta(arg_h->socket_memoria, OK);
                    }
                }
                
                free(contenido);

                break;
	        case LEER_SWAP:
                uint32_t bloque;
                void* contenido = malloc(tam_bloque);
                recv(arg_h->socket_memoria, &bloque, sizeof(uint32_t), MSG_WAITALL);
                if(bloque != 0){
                    contenido = leer_bloque_swap(bloque);
                    send(arg_h->socket_memoria, &contenido, (size_t)tam_bloque, MSG_WAITALL);
                }

                free(contenido);
                break;
	        case ELIMINAR_SWAP:
                uint32_t pid;
                recv(arg_h->socket_memoria, &pid, sizeof(uint32_t), NULL);
                eliminar_en_swap(pid);
                enviar_mensaje(arg_h->socket_memoria, OK);

                break;
        default:
            list_destroy_and_destroy_elements(lista_de_bloques_swap, free);
            liberar_conexion(arg_h->socket_memoria);
            return;
        }

    }

}

void reservar_bloques_swap(uint32_t cantidad, uint32_t pid){
    uint32_t bloque;
    int i;
    for(i = 0; i < cantidad; i++){
        bloque = reservar_bloque_swap(pid);
        printf("Se reservo el bloque %d para el proceso %d en SWAP", bloque, pid);
    }
}

uint32_t reservar_bloque_swap(uint32_t pid){
    uint32_t bloque = elegir_bloque_swap();
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

uint32_t elegir_bloque_swap(){
    int i;
    t_bloque_swap_info* bloque_info;
    for(i=0; i < swap_bitarray->size * 8; i++){
        if(!bitarray_test_bit(swap_bitarray,i))
            break;
    }
    if(i <= swap_bitarray->size * 8){
        bloque_info = list_get(lista_de_bloques_swap, 0);
        i = bloque_info->bloque;
        list_remove_and_destroy_element(lista_de_bloques_swap, 0, free);
    }
    return i;
}

void* leer_bloque_swap(uint32_t bloque){
    void* bloque = malloc(tam_bloque);
    fseek(bloques, bloque * tam_bloque, SEEK_SET);
    fread(&bloque, tam_bloque, 1, bloques);
    return bloque;
}

void escribir_bloque_swap(uint32_t bloque, void* contenido){
    fseek(bloques, bloque * tam_bloque, SEEK_SET);
    fwrite(&contenido, (size_t)tam_bloque, 1, bloques);
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