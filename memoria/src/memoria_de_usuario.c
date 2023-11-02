#include "memoria_de_usuario.h"

extern void* memoria_de_usuario;
extern t_bitarray* frame_bitarray;
extern t_list* procesos_en_memoria;

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
    t_pagina* pagina = list_find(proceso->tabla_de_paginas, es_la_pagina);
}

//Envía el número de 
void enviar_frame (int socket, uint32_t frame)
{
    op_code operacion = FRAME;
    
    send(socket, &operacion, sizeof(op_code), NULL);
}