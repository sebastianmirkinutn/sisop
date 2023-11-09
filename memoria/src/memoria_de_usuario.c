#include "memoria_de_usuario.h"

extern void* memoria_de_usuario;
extern t_bitarray* frame_bitarray;
extern t_list* procesos_en_memoria;
extern sem_t mutex_lista_procesos;
extern sem_t cantidad_de_procesos;
extern int tam_pagina;

uint8_t leer_de_memoria(t_direccion_fisica* direccion)
{
    uint8_t leido;
    memcpy(&leido, memoria_de_usuario + (direccion->frame * tam_pagina) + direccion->offset, sizeof(uint8_t));
    return leido;
}

void escribir_en_memoria(t_direccion_fisica* direccion, uint8_t a_escribir)
{
    memcpy(memoria_de_usuario + (direccion->frame * tam_pagina) + direccion->offset, &a_escribir, sizeof(uint8_t));
}

int32_t obtener_numero_de_marco(uint32_t pid, uint32_t pagina_buscada)
{
/*
    printf("obtener_numero_de_marco\n");
    bool es_el_pid(void* arg){
        t_proceso* proceso = (t_proceso*)arg;
        return proceso->pid == pid;
    }
*/
    bool es_la_pagina(void* arg){
        t_pagina* pagina = (t_pagina*)arg;
        return pagina->pagina == pagina_buscada;
    }
    t_proceso* proceso = buscar_proceso(pid);
    if(proceso != NULL)
    {
        printf("Existe el proceso del que se busca el marco\n");
        t_pagina* pagina = list_find(proceso->tabla_de_paginas, es_la_pagina);
        if(pagina != NULL)
        {
            printf("El frame de la página %i del proceso %i es %i\n", pagina_buscada, pid, pagina->frame);
            return pagina->frame;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        printf("No existe el proceso del que se busca el marco\n");
        //Page fault
        return -1;
    }
}

void asignar_memoria(uint32_t pid, uint32_t size, uint32_t (*algoritmo)(void))
{
    uint32_t frame;
    for(int nro_pagina = 0; nro_pagina < size / tam_pagina /*Habría que redondear hacia arriba*/; nro_pagina++)
    {
        frame = algoritmo();
        printf("Se asigna el frame %i\n", frame);
        agregar_pagina(pid, nro_pagina, frame);
        printf("Se asigna el frame %i al proceso %i para la página %i\n", frame, pid, nro_pagina);
    }
}

void agregar_pagina(uint32_t pid, uint32_t nro_pagina, uint32_t nro_frame)
{
    bool tiene_mismo_pid(void* proceso) {
        return (((t_proceso*)proceso)->pid == pid);
    }

    t_proceso* proceso;
    proceso = list_find(procesos_en_memoria, tiene_mismo_pid);
    
    if(proceso != NULL)
    {
        printf("Se crea una página\n");
        t_pagina* pagina = crear_pagina(nro_pagina, nro_frame);
        printf("Se creó una página (p = %i - f = %i)\n", pagina->pagina, pagina->frame);
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

/*ALGORTMOS DE REEMPLAZO*/

uint32_t contador_frame = 0;
uint32_t reemplazo_fifo(void)
{
    return contador_frame;
    contador_frame++;
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
    sem_wait(&mutex_lista_procesos);
    printf("hice waitss BUCAR_PROCESO\n");
    //pid_funcion = pid;
    proceso = list_find(procesos_en_memoria, comparar);
    sem_post(&mutex_lista_procesos);
    printf("TERMINA BUCAR_PROCESO\n");
    return proceso;
}