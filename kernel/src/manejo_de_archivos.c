#include "manejo_de_archivos.h"

extern sem_t grado_de_multiprogramacion;
extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_exit;
extern sem_t procesos_en_new;
extern sem_t procesos_en_ready;
extern sem_t planificacion_largo_plazo;
extern sem_t planificacion_corto_plazo;
extern sem_t mutex_file_management;
extern sem_t mutex_tabla_global_de_archivos;;
  
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
    archivo->nombre = malloc(strlen(nombre_archivo) + 1);
    strcpy(archivo->nombre, nombre_archivo);
    archivo->puntero = 0;
    archivo->lock = lock;
    archivo->cola_blocked = queue_create();
    archivo->locks_lectura = list_create();
    sem_init(&(archivo->mutex_cola_blocked), 0, 1);
    sem_init(&(archivo->mutex_locks_lectura), 0, 1);
    archivo->contador_aperturas = 1;
    return archivo;
}

t_lock de_string_a_t_lock(char* str)
{
    t_lock lock = NONE;
    if(str[0] == 'R')
    {
        lock = READ;
    }
    else if(str[0] == 'W')
    {
        lock = WRITE;
    }
    return lock;
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
    sem_wait(&mutex_file_management);
    t_args_hilo_archivos* arg_h = (t_args_hilo_archivos*) arg;
    t_response respuesta;

    t_archivo* archivo =  buscar_archivo(tabla_global_de_archivos, arg_h->nombre_archivo);
    if(archivo != NULL)
    {
        printf("archivo != NULL\n");
        
        
        if(archivo->lock == READ)
        {
            printf("archivo->lock == READ");
            if(arg_h->lock == READ)
            {
                archivo->contador_aperturas++;
                printf("arg_h->lock == READ");
                sem_wait(&(archivo->mutex_locks_lectura));
                list_add(archivo->locks_lectura, execute); //no se bloquea el proceso
                sem_post(&(archivo->mutex_locks_lectura));
                sem_wait(&mutex_cola_ready);
                printf("execute = %i - arg_h->execute = %i\n",execute->pid, arg_h->execute->pid);
                arg_h->execute->estado = READY;
                agregar_primero_en_cola(cola_ready, arg_h->execute); // Debería ser en la primera posición.
                sem_post(&mutex_cola_ready);
                sem_post(&procesos_en_ready);
            
            } 
            else 
            {
                printf("arg_h->lock != READ");
                sem_wait(&(archivo->mutex_cola_blocked));
                list_add(archivo->cola_blocked, execute);
                sem_post(&(archivo->mutex_cola_blocked));
                arg_h->execute->estado = BLOCKED;
                sem_post(&mutex_file_management);
                printf("TERMINA EL HILO\n");
                liberar_parametros(arg_h);
                return;
            }
       
        }
        else if(arg_h->lock == WRITE)
        {
            int temp;
            sem_getvalue(&(archivo->mutex_cola_blocked), &temp);
            printf("archivo->mutex_cola_blocked = %i\n", temp);
            sem_wait(&(archivo->mutex_cola_blocked));
            list_add(archivo->cola_blocked, execute);
            sem_post(&(archivo->mutex_cola_blocked));
            execute->estado = BLOCKED;
            sem_post(&mutex_file_management);
            printf("TERMINA EL HILO\n");
            liberar_parametros(arg_h);
            return;
        }
        else //Ningún archivo lo tiene abierto
        {
            archivo->contador_aperturas++;
            archivo->lock = READ;
            printf("arg_h->lock == READ");
            sem_wait(&(archivo->mutex_locks_lectura));
            list_add(archivo->locks_lectura, execute); //no se bloquea el proceso
            sem_post(&(archivo->mutex_locks_lectura));
            sem_wait(&mutex_cola_ready);
            printf("execute = %i - arg_h->execute = %i\n",execute->pid, arg_h->execute->pid);
            agregar_primero_en_cola(cola_ready, arg_h->execute); // Debería ser en la primera posición.
            printf("1");
            sem_post(&mutex_cola_ready);
            printf("2");
            sem_post(&procesos_en_ready);
            printf("3");
            sem_post(&mutex_file_management);
            liberar_parametros(arg_h);
            return;
        }
        
    }
    else
    {
        printf("archivo == NULL\n");
        archivo = crear_archivo(arg_h->nombre_archivo, arg_h->tam_archivo, arg_h->lock); 
        printf("archivo == %i\n", archivo);
        printf("archivo->nombre == %s\n", archivo->nombre);
        sem_wait(&mutex_tabla_global_de_archivos);
        list_add(tabla_global_de_archivos, archivo);
        sem_post(&mutex_tabla_global_de_archivos);

        list_add(execute->tabla_de_archivos_abiertos, archivo);
        log_info(arg_h->logger, "Agregué el archivo %s en %i", archivo->nombre, tabla_global_de_archivos);
    
  
    
    
        enviar_operacion(arg_h->socket_filesystem, ABRIR_ARCHIVO);
        enviar_mensaje(arg_h->nombre_archivo, arg_h->socket_filesystem); //SERIALIZAR
        recv(arg_h->socket_filesystem, &(arg_h->tam_archivo), sizeof(int32_t), MSG_WAITALL);
        if(arg_h->tam_archivo != -1)
        {
            printf("El archivo tiene un tamaño de %i bytes (lock = %i)\n", arg_h->tam_archivo, arg_h->lock);
            execute->estado = READY;
            sem_wait(&mutex_cola_ready);
            agregar_primero_en_cola(cola_ready, arg_h->execute); // Debería ser en la primera posición.
            sem_post(&mutex_cola_ready);
            sem_post(&procesos_en_ready);
        }
        else
        {
            printf("El archivo no existe\n");
            //Se le pide a Filesystem que cree el archivo
            enviar_operacion(arg_h->socket_filesystem, CREAR_ARCHIVO);
            enviar_mensaje(arg_h->nombre_archivo, arg_h->socket_filesystem); //SERIALIZAR
            printf("Mandé el nombre del archivo\n");
            //Podríamos recibir un OK, de hecho creo que hay que recibirlo
            respuesta = recibir_respuesta(arg_h->socket_filesystem);
            switch (respuesta)
            {
                case OK:
                    sem_wait(&mutex_cola_ready);
                    agregar_primero_en_cola(cola_ready, arg_h->execute); // Debería ser en la primera posición.
                    sem_post(&mutex_cola_ready);
                    sem_post(&procesos_en_ready);
                    break;
                default:
                    break;
            }
        }
    }
    sem_post(&mutex_file_management);
    printf("TERMINA EL HILO\n");
    liberar_parametros(arg_h);
    return;
}

void file_read(void* arg)
{
    sem_wait(&mutex_file_management);
    t_args_hilo_archivos* arg_h = (t_args_hilo_archivos*) arg;
    t_response respuesta;

    enviar_operacion(arg_h->socket_filesystem, LEER_ARCHIVO);
    enviar_mensaje(arg_h->nombre_archivo, arg_h->socket_filesystem); //SERIALIZAR (OP, NOMBRE_ARCHIVO, PUNTERO)
    t_archivo* archivo = buscar_archivo(tabla_global_de_archivos, arg_h->nombre_archivo);
    send(arg_h->socket_filesystem, &(archivo->puntero), sizeof(uint32_t),0);
    enviar_direccion(arg_h->socket_memoria, arg_h->direccion); //SERIALIZAR NO HACE FALTA
    //recv(arg_h->socket_filesystem, &tam_archivo, sizeof(int32_t), MSG_WAITALL);

    sem_wait(&mutex_cola_ready);
    agregar_primero_en_cola(cola_ready, arg_h->execute); // Debería ser en la primera posición.
    sem_post(&mutex_cola_ready);
    sem_post(&procesos_en_ready);

    sem_post(&mutex_file_management);
    liberar_parametros(arg_h);
}

void file_write(void* arg)
{
    sem_wait(&mutex_file_management);
    t_args_hilo_archivos* arg_h = (t_args_hilo_archivos*) arg;
    t_response respuesta;
    printf("Direccion = %i:%i\n", arg_h->direccion->frame, arg_h->direccion->offset);
    enviar_operacion(arg_h->socket_filesystem, ESCRIBIR_ARCHIVO);
    enviar_mensaje(arg_h->nombre_archivo, arg_h->socket_filesystem);
    t_archivo* archivo = buscar_archivo(tabla_global_de_archivos, arg_h->nombre_archivo);
    send(arg_h->socket_filesystem, &(archivo->puntero), sizeof(uint32_t),0);
    enviar_direccion(arg_h->socket_filesystem, arg_h->direccion); //SERIALIZAR (OPERACION, NOMBRE, PUNTERO, DIRECCION)
    respuesta = recibir_respuesta(arg_h->socket_filesystem);
    switch (respuesta)
        {
            case OK:
                printf("OK escritura\n");
                sem_wait(&mutex_cola_ready);
                agregar_primero_en_cola(cola_ready, arg_h->execute); // Debería ser en la primera posición.
                //enviar_operacion(arg_h->socket_interrupt, CLOCK_INTERRUPT); //Cambiar a I/O interrupt
                sem_post(&mutex_cola_ready);
                sem_post(&procesos_en_ready);
                break;
            default:
                break;
        }

    sem_post(&mutex_file_management);
    liberar_parametros(arg_h);
}

void file_truncate(void* arg)
{
    printf("Empieza file_truncate()\n");
    sem_wait(&mutex_file_management);
    t_args_hilo_archivos* arg_h = (t_args_hilo_archivos*) arg;
    t_response respuesta;
    printf("PRINT");
    printf("Pido truncar %s a %u", arg_h->nombre_archivo, arg_h->tam_archivo);
    enviar_operacion(arg_h->socket_filesystem, TRUNCAR_ARCHIVO);
    enviar_mensaje(arg_h->nombre_archivo, arg_h->socket_filesystem);
    send(arg_h->socket_filesystem, &(arg_h->tam_archivo), sizeof(uint32_t), NULL);  //SERIALIZAR
    respuesta = recibir_respuesta(arg_h->socket_filesystem);
        switch (respuesta)
        {
            case OK:
            printf("OK\n");
            int temp;
            sem_getvalue(&mutex_cola_ready, &temp);
            printf("mutex_cola_ready = %i\n", temp); 
                sem_wait(&mutex_cola_ready);
                agregar_primero_en_cola(cola_ready, arg_h->execute); // Debería ser en la primera posición.
                sem_post(&mutex_cola_ready);
                sem_post(&procesos_en_ready);
                break;
            default:
                break;
        }
    sem_post(&mutex_file_management);
    printf("TERMINA EL HILO\n");
    liberar_parametros(arg_h);
}

void file_close(void* arg)
{
    printf("file_close()\n");
    sem_wait(&mutex_file_management);
    printf("ejecuta file_close()\n");
    t_args_hilo_archivos* arg_h = (t_args_hilo_archivos*) arg;
    t_response respuesta;
 
    sem_wait(&mutex_cola_ready);
    agregar_primero_en_cola(cola_ready, arg_h->execute); // Debería ser en la primera posición.
    sem_post(&mutex_cola_ready);
    sem_post(&procesos_en_ready);
    sem_post(&mutex_file_management);
}

void file_seek(void* arg)
{
    t_args_hilo_archivos* arg_h = (t_args_hilo_archivos*) arg;
    t_response respuesta;
 
    sem_wait(&mutex_file_management);
    log_info(arg_h->logger, "Busco el archivo %s en %i", arg_h->nombre_archivo, tabla_global_de_archivos);
    sem_wait(&mutex_tabla_global_de_archivos);
    t_archivo* archivo = buscar_archivo(tabla_global_de_archivos, arg_h->nombre_archivo);
    sem_post(&mutex_tabla_global_de_archivos);
    if(archivo != NULL)
    {
        archivo->puntero = arg_h->puntero;
        log_info(arg_h->logger, "Archivo: %i - Puntero: %i", archivo->nombre, arg_h->puntero);
        sem_wait(&mutex_cola_ready);
        agregar_primero_en_cola(cola_ready, execute);
        sem_post(&mutex_cola_ready);
        sem_post(&procesos_en_ready);
    }
    else
    {
        log_error(arg_h->logger, "El archivo no existe");
    }
    sem_post(&mutex_file_management);
}