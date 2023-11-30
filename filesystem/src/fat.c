#include "fat.h"

extern t_fat* fat;

extern uint32_t cant_bloques_total;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern uint32_t retardo_acceso_bloque;
extern uint32_t retardo_acceso_fat;

t_fat* crear_fat_mapeada(char* path, uint32_t size)
{
    uint32_t initialisation_value = UINT32_MAX;
    uint32_t offset = 0;
    t_fat* fat = malloc(sizeof(t_fat));
    fat->file_descriptor = open(path, O_RDWR);
    struct stat file_stat;
    fstat(fat->file_descriptor, &file_stat);
    off_t file_size = file_stat.st_size;
    printf("size = %li\n", (cant_bloques_total- cant_bloques_swap)* sizeof(uint32_t));
    printf("FILE_SIZE = %li\n", file_size);
    if(fat->file_descriptor == -1)
	{
		fat->file_descriptor = open(path, O_CREAT | O_RDWR, (mode_t) 0664);
	}
    if(fat->file_descriptor != -1)
    {
        ftruncate(fat->file_descriptor, (cant_bloques_total- cant_bloques_swap)* sizeof(uint32_t));
        fat->memory_map = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fat->file_descriptor, 0);
        memcpy(fat->memory_map, &initialisation_value, sizeof(uint32_t));
        initialisation_value = 0;
        for(uint32_t i = 1; i < (cant_bloques_total- cant_bloques_swap)/4 ; i++)
        {
            //memcpy(fat->memory_map + offset, &initialisation_value, sizeof(uint32_t));
            //offset += sizeof(uint32_t);
            fat->memory_map[i] = initialisation_value;
        }
    }
    else
    {
        // Error
    }
    return fat;
}

uint32_t obtener_bloque_libre()
{
    for(uint32_t i = 0; i < cant_bloques_total - cant_bloques_swap; i++)
    {
        if(fat->memory_map[i] == 0)
        {
            return i;
        }
    }
    return UINT32_MAX;
}

FILE* crear_fat(char* path, uint32_t size)
{
    //FILE* fat = fopen(path, "rw")
}