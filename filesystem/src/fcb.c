#include "fcb.h"

t_fcb* crear_fcb()
{
    t_fcb* fcb = malloc(sizeof(t_fcb));
    return fcb;
}

void liberar_fcb(t_fcb* fcb)
{
    if(fcb->nombre != NULL)
    {
        free(fcb->nombre);
    }
    free(fcb);
}

t_fcb* leer_fcb(char* path_fcb, char* nombre)
{
    t_fcb* fcb = crear_fcb();
    char* ruta = path_fcb;
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
    t_config* archivo_fcb = iniciar_config(ruta);
    fcb->nombre = config_get_string_value(archivo_fcb, "NOMBRE_ARCHIVO");
    fcb->tam_archivo = config_get_int_value(archivo_fcb, "TAMANIO_ARCHIVO");
    fcb->bloque_inicial = config_get_int_value(archivo_fcb, "BLOQUE_INICIAÑ");
    return fcb;
}