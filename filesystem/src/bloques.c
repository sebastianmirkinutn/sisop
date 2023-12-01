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

void escribir_dato(uint32_t bloque, uint32_t offset, uint32_t dato)
{
    fseek(bloques, (cant_bloques_swap + bloque) * tam_bloque + offset, SEEK_SET);
    fwrite(&dato, sizeof(uint32_t), 1, bloques);
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
