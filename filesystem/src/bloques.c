#include "bloques.h"

extern uint32_t cant_bloques_total;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern uint32_t retardo_acceso_bloque;
extern uint32_t retardo_acceso_fat;


extern FILE* bloques;

void* leer_bloque(uint32_t puntero)
{
    void* bloque = malloc(tam_bloque);
    fseek(bloques, (cant_bloques_swap + puntero) * tam_bloque, SEEK_SET);
    fread(&bloque, tam_bloque, 1, bloques);
    return bloque;
}
/*
void escribir(uint32_t puntero, )
{
    void* bloque = malloc(tam_bloque);
    fseek(bloques, (cant_bloques_swap + puntero) * tam_bloque, SEEK_SET);
    fwrite()
    return bloque;
}
*/
