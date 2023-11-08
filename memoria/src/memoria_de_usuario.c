#include "memoria_de_usuario.h"

extern void* memoria_de_usuario;
extern t_bitarray* frame_bitarray;
extern t_list* procesos_en_memoria;
extern int tam_pagina;

uint8_t leer_de_memoria(uint32_t direccion)
{
    uint8_t leido;
    memcpy(&leido, memoria_de_usuario + direccion, sizeof(uint8_t));
    return leido;
}

void escribir_en_memoria(uint32_t direccion, uint8_t byte)
{
    memcpy(memoria_de_usuario + direccion, &byte,sizeof(uint8_t));
}

int32_t obtener_numero_de_marco(uint32_t pid, uint32_t pagina_buscada)
{
    bool es_el_pid(void* arg){
        t_proceso* proceso = (t_proceso*)arg;
        return proceso->pid == pid;
    }

    bool es_la_pagina(void* arg){
        t_proceso* proceso = (t_proceso*)arg;
        return proceso->tabla_de_paginas == pagina_buscada;
    }

    t_proceso* proceso = list_find(procesos_en_memoria, es_el_pid);
    if(proceso != NULL)
    {
        t_pagina* pagina = list_find(proceso->tabla_de_paginas, es_la_pagina);
        if(pagina != NULL)
        {
            return pagina->frame;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        //Page fault
        return -1;
    }
}

void asignar_memoria(uint32_t pid, uint32_t size, uint32_t (*algoritmo)(void))
{
    uint32_t frame;
    /*
    for(int nro_pagina = 0; nro_pagina < size / tam_pagina; nro_pagina++)
    {
        frame = algoritmo();
        agregar_pagina(pid, nro_pagina, frame);
    }
    */
}

void agregar_pagina(uint32_t pid, uint32_t nro_pagina, uint32_t nro_frame)
{
    t_proceso* proceso;
    t_pagina* pagina = crear_pagina(nro_pagina, nro_frame);
    list_add(proceso->tabla_de_paginas, pagina);
}

/*ALGORTMOS DE REEMPLAZO*/

uint32_t contador_frame;
uint32_t reemplazo_fifo(void)
{
    contador_frame++;
    return contador_frame;
}