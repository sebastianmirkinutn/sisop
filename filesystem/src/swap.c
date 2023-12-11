#include "swap.h"

extern FILE* bloques;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern t_bitarray* swap_bitarray;
extern t_log* logger;

void conexion_memoria(void* arg)
{
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    uint32_t pid;
    uint32_t cantidad_de_bloques;
    uint32_t nro_bloque;
    while(1)
    {
        op_code codigo = recibir_operacion(arg_h->socket_swap);
        switch (codigo)
        {
            case RESERVAR_BLOQUES_SWAP:
                recv(arg_h->socket_swap, &cantidad_de_bloques, sizeof(uint32_t), MSG_WAITALL);
                log_info(logger, "Pide reservar %i bloques", cantidad_de_bloques);
                reservar_bloques_swap(cantidad_de_bloques);
                log_info(logger, "Se reservaron.");
                //enviar los bloques
                break;

	        case ESCRIBIR_SWAP:

                
                break;

	        case LEER_SWAP:
                recv(arg_h->socket_swap, &nro_bloque, sizeof(uint32_t), MSG_WAITALL);
   
                //Enviar bloque

                break;

	        case LIBERAR_BLOQUES_SWAP:
                recv(arg_h->socket_swap, &nro_bloque, sizeof(uint32_t), MSG_WAITALL);

                enviar_respuesta(arg_h->socket_swap, OK);
                break;

        default:
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
    uint32_t bloque = elegir_bloque_swap();
    bitarray_set_bit(swap_bitarray, bloque);
    return bloque;
}

uint32_t elegir_bloque_swap()
{
    int i;
    for(i = 0; i < swap_bitarray->size * 8; i++)
    {
        if(!bitarray_test_bit(swap_bitarray,i))
        {
            break;
        }
    }
    return i;
}

void* leer_bloque_swap(uint32_t nro_bloque)
{
    void* bloque = malloc(tam_bloque);
    fseek(bloques, nro_bloque * tam_bloque, SEEK_SET);
    fread(bloque, tam_bloque, 1, bloques);
    return bloque;
}

void escribir_bloque_swap(uint32_t nro_bloque, void* contenido)
{
    fseek(bloques, nro_bloque * tam_bloque, SEEK_SET);
    fwrite(contenido, tam_bloque, 1, bloques);
}