#include "page_fault.h"

extern void* memoria_de_usuario;
extern t_bitarray* frame_bitarray;
extern t_list* procesos_en_memoria;
extern sem_t mutex_lista_procesos;
extern sem_t cantidad_de_procesos;
extern int tam_pagina;
extern int tam_memoria;
extern t_log* logger;

/*ALGORTMOS DE REEMPLAZO*/
uint32_t contador_frame = 0;
uint32_t buscar_victima_fifo(void)
{
    uint32_t victima = contador_frame;
    contador_frame++;
    //Lógica de desalojo

    if(contador_frame > tam_memoria / tam_pagina)
    {
        contador_frame = 0;
    }
    return victima;
}

int32_t buscar_frame_libre()
{
    uint32_t i;
    for(i = 0; i < tam_memoria / tam_pagina; i++)
    {
        if(bitarray_test_bit(frame_bitarray, i))
        {
            return i;
        }
    }
    return -1;
}

/*
uint32_t buscar_victima_lru(void)
{
    uint32_t victima = contador_frame;

    contador_frame++;
    //Lógica de desalojo

    if(contador_frame > tam_memoria / tam_pagina)
    {
        victima = ultima_pagina_accedida();
    }
    //for(uint32_t i; i < tam_memoria / tam_pagina)

    //bitarray_set_bit(frame_bitarray, victima);
    return victima;
}

uint32_t ultima_pagina_accedida()
{
    bool es_menor_el_timestamp(void* e1, void* e2)
    {
        return(((t_pagina*)e1)->timestamp > ((t_pagina*)e2)->timestamp);
    }
    bool el_menor_timestamp(void* e1, void* e2)
    {
        t_pagina* p1, p2;
        p1 = list_find(((t_proceso*)e1)->tabla_de_paginas, es_menor_el_timestamp);
    }
    t_proceso* respuesta = (t_proceso*)list_find(procesos_en_memoria, el_menor_timestamp);
    return list_find(respuesta->tabla_de_paginas, es_menor_el_timestamp);
}
*/

void* leer_pagina(uint32_t nro_frame)
{
    void* leido = malloc(tam_pagina);
    memcpy(&leido, memoria_de_usuario + (nro_frame * tam_pagina), tam_pagina);
    return leido;
}

void escribir_pagina(uint32_t nro_frame, void* a_escribir)
{
    memcpy(memoria_de_usuario + (nro_frame * tam_pagina) , a_escribir, tam_pagina);
}

void swap_in(int socket_swap, t_pagina* pagina, uint32_t frame)
{
    void* a_escribir = malloc(tam_pagina);
    enviar_operacion(socket_swap, LEER_SWAP);
    send(socket_swap, &(pagina->posicion_en_swap), sizeof(uint32_t), NULL);
    recv(socket_swap, a_escribir, tam_pagina, MSG_WAITALL);
    escribir_pagina(frame, a_escribir);
    free(a_escribir);
}

void swap_out(int socket_swap, t_pagina* pagina, uint32_t frame, void* a_escribir)
{
    t_response respuesta;
    enviar_operacion(socket_swap, ESCRIBIR_SWAP);
    send(socket_swap, &(pagina->posicion_en_swap), sizeof(uint32_t), NULL);
    send(socket_swap, a_escribir, tam_pagina, NULL);
    respuesta = recibir_respuesta(socket_swap);
}