#include "manejo_de_archivos.h"

extern sem_t grado_de_multiprogramacion;
extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_exit;
extern sem_t procesos_en_new;
extern sem_t procesos_en_ready;
extern sem_t planificacion_largo_plazo;
extern sem_t planificacion_corto_plazo;
  
extern t_queue *cola_ready;
extern t_queue *cola_exit;
extern t_queue *cola_new;
 
extern t_pcb* execute;
extern t_list* recursos_disponibles;
extern t_list* tabla_global_de_archivos;

t_archivo* crear_archivo(char* nombre_archivo, uint32_t tam_archivo, t_lock lock)
{
    t_archivo* archivo = malloc(sizeof(t_archivo));
    archivo->cola_blocked = queue_create();
    archivo->tam_archivo = tam_archivo;
    archivo->nombre = nombre_archivo;
    archivo->puntero = 0;
    archivo->lock = lock;
    return archivo;
}

t_lock de_string_a_t_lock(char* str)
{
    t_lock lock = NONE;
    if(!strcmp(str, "R"))
    {
        lock = READ;
    }
    else if(!strcmp(str, "W"))
    {
        lock = WRITE;
    }
}

t_archivo* buscar_archivo(t_list* lista, char* nombre)
{
    bool tiene_el_mismo_nombre(void* arg)
    {
        t_archivo* archivo = (t_archivo*) arg;
        return (!strcmp(archivo->nombre, nombre));
    }
    t_archivo* archivo = NULL;
    archivo = list_find(lista, tiene_el_mismo_nombre);
    return archivo;
}

void file_open(void* arg)
{
    t_args_hilo* arg_h = (t_args_hilo*) arg;
    int32_t tam_archivo;
    t_response respuesta;

    t_archivo* archivo = crear_archivo(arg_h->nombre_archivo, tam_archivo, de_string_a_t_lock(arg_h->lock));
    list_add(tabla_global_de_archivos, archivo);
    list_add(execute->tabla_de_archivos_abiertos, archivo);
    enviar_operacion(arg_h->socket_filesystem, ABRIR_ARCHIVO);
    enviar_mensaje(arg_h->nombre_archivo, arg_h->socket_filesystem);
    recv(arg_h->socket_filesystem, &tam_archivo, sizeof(int32_t), MSG_WAITALL);
    if(tam_archivo != -1)
    {
        printf("El archivo tiene un tamaño de %i bytes\n", tam_archivo);
        sem_wait(&mutex_cola_ready);
        queue_push(cola_ready, execute); // Debería ser en la primera posición.
        sem_post(&mutex_cola_ready);
        sem_post(&procesos_en_ready);
    }
    else
    {
        printf("El archivo no existe\n");
        //Se le pide a Filesystem que cree el archivo
        enviar_operacion(arg_h->socket_filesystem, CREAR_ARCHIVO);
        enviar_mensaje(arg_h->nombre_archivo, arg_h->socket_filesystem);
        printf("Mandé el nombre del archivo\n");
        //Podríamos recibir un OK, de hecho creo que hay que recibirlo
        respuesta = recibir_respuesta(arg_h->socket_filesystem);
        switch (respuesta)
        {
        case OK:
            sem_wait(&mutex_cola_ready);
            queue_push(cola_ready, execute); // Debería ser en la primera posición.
            sem_post(&mutex_cola_ready);
            sem_post(&procesos_en_ready);
            break;
        default:
            break;
        }
    }
}