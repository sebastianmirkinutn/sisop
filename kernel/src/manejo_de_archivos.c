#include "manejo_de_archivos.h"

t_archivo* crear_archivo(char* nombre_archivo, uint32_t tam_archivo, char* lock)
{
    t_archivo* archivo = malloc(sizeof(t_archivo));
    archivo->cola_blocked = queue_create();
    archivo->tam_archivo = tam_archivo;
    archivo->nombre = nombre_archivo;
    archivo->puntero = 0;
    archivo->lock = NONE; //Habría que comparar con lock (char*)
    return archivo;
}