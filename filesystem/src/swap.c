#include "swap.h"

extern FILE* bloques;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern t_bitarray* swap_bitarray;
extern t_log* logger;
extern uint32_t tam_pagina;
extern uint32_t retardo_acceso_bloque;

void conexion_memoria(void* arg)
{
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    uint32_t pid;
    uint32_t cantidad_de_bloques;
    uint32_t nro_bloque;
    void* bloque_swap;
    t_list* bloques;
    while(1)
    {
        op_code codigo = recibir_operacion(arg_h->socket_swap);
        printf("Codigo = %i", codigo);
        switch (codigo)
        {
            case RESERVAR_BLOQUES_SWAP:
            printf("RESERVAR_BLOQUES_SWAP\n");
                recv(arg_h->socket_swap, &cantidad_de_bloques, sizeof(uint32_t), MSG_WAITALL);
                {
                    log_info(logger, "RESERVAR_BLOQUES_SWAP - Pide reservar %i bloques", cantidad_de_bloques);
                    t_list* bloques = reservar_bloques_swap(cantidad_de_bloques);
                    log_info(logger, "RESERVAR_BLOQUES_SWAP - Se reservaron bloques.");

                    for(int i = 0; i < cantidad_de_bloques; i++)
                    {
                        uint32_t* elemento = list_get(bloques, i);
                        send(arg_h->socket_swap, elemento, sizeof(uint32_t), NULL);
                        printf("Envío: %i\n", *elemento);
                    }
                    //printf("Salió del for\n");
                }
                //enviar los bloques
                break;

	        case ESCRIBIR_SWAP:
                printf("ESCRIBIR_SWAP\n");
                printf("tam_pagina = %i\n", tam_pagina);
                recv(arg_h->socket_swap, &nro_bloque, sizeof(uint32_t), MSG_WAITALL);
                printf("bloque %i\n", nro_bloque);
                recv(arg_h->socket_swap, bloque_swap, tam_pagina, MSG_WAITALL);
                printf("bloque %i\n", nro_bloque);
                escribir_bloque_swap(nro_bloque, bloque_swap);
                //printf("Ya lo escribió\n");
                enviar_respuesta(arg_h->socket_swap, OK);
                //printf("Termino de ESCRIBIR_SWAP\n");
                log_info(logger,"ESCRIBIR_SWAP - Escribe bloque: %i de datos swap",nro_bloque);
                
                break;

	        case LEER_SWAP:
            printf("LEER_SWAP\n");
                recv(arg_h->socket_swap, &nro_bloque, sizeof(uint32_t), MSG_WAITALL);
                bloque_swap = leer_bloque_swap(nro_bloque);
                send(arg_h->socket_swap, bloque_swap, tam_pagina, NULL);
                log_info(logger,"LEER_SWAP - Lee bloque de datos swap");
                //Enviar bloque

                break;

	        case LIBERAR_BLOQUES_SWAP:
                printf("LIBERAR_BLOQUES_SWAP\n");
                recv(arg_h->socket_swap, &cantidad_de_bloques, sizeof(uint32_t), MSG_WAITALL);
                printf("Necesito liberar %i bloques\n", cantidad_de_bloques);
                for(int i = 0; i < cantidad_de_bloques; i++)
                    {
                        uint32_t elemento;
                        recv(arg_h->socket_swap, &elemento, sizeof(uint32_t), MSG_WAITALL);
                        printf("Libero: %i\n", elemento);
                        log_info(logger,"LIBERAR_BLOQUES_SWAP - se liberó:", elemento);
                        bitarray_clean_bit(swap_bitarray, elemento);
                    }

                enviar_respuesta(arg_h->socket_swap, OK);
                break;

        default:

            log_error(logger, "Código inválido");
            //list_destroy_and_destroy_elements(lista_de_bloques_swap, free);
            //liberar_conexion(arg_h->socket_swap);
            return;
        }

    }

}

t_list* reservar_bloques_swap(uint32_t cantidad)
{
    t_list* bloques = list_create();
    uint32_t* bloque;
    for(int i = 0; i < cantidad; i++)
    {
        log_info(logger,"reservo bloque %i", i);
        bloque = malloc(sizeof(uint32_t));
        *bloque = reservar_bloque_swap();
        list_add(bloques, bloque);
        log_info(logger, "Se reservo el bloque %i.", *bloque);
    }
    return bloques;
}

uint32_t reservar_bloque_swap()
{
    uint32_t bloque = buscar_bloque_swap_libre();
    if(bloque != -1)
    {
        bitarray_set_bit(swap_bitarray, bloque);
    }
    return bloque;
}

uint32_t buscar_bloque_swap_libre()
{
    int i;
    for(i = 0; i < swap_bitarray->size * 8; i++)
    {
        if(!bitarray_test_bit(swap_bitarray,i))
        {
            log_info(logger, "BUSCAR BLOQUE SWAP LIBRE - bloque:  %i",i);
            return i;
        }
    }
    return -1;
}

void* leer_bloque_swap(uint32_t nro_bloque)
{
    void* bloque = malloc(tam_pagina);
    fseek(bloques, nro_bloque * tam_pagina, SEEK_SET);
    fread(bloque, tam_pagina, 1, bloques);
    sleep(retardo_acceso_bloque / 1000);
    log_info(logger, "LEER BLOQUE SWAP - bloque:  %i",bloque);
    return bloque;
}

void escribir_bloque_swap(uint32_t nro_bloque, void* contenido)
{
    fseek(bloques, nro_bloque * tam_pagina, SEEK_SET);
    fwrite(contenido, tam_pagina, 1, bloques);
    sleep(retardo_acceso_bloque / 1000);
}