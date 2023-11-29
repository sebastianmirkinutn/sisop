#include "fat.h"

extern t_fat* fat;

t_fat* crear_fat_mapeada(char* path, uint32_t size)
{
    t_fat* fat = malloc(sizeof(t_fat));
    fat->file_descriptor = open(path, O_RDWR);
    fat->memory_map = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fat->file_descriptor, 0);
    return fat;
}