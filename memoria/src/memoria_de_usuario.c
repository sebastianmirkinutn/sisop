#include "memoria_de_usuario.h"

extern void* memoria_de_usuario;
extern t_bitarray* frame_bitarray;

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