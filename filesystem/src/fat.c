#include "fat.h"

extern t_fat* fat;

extern uint32_t cant_bloques_total;
extern uint32_t cant_bloques_swap;
extern uint32_t tam_bloque;
extern uint32_t retardo_acceso_bloque;
extern uint32_t retardo_acceso_fat;

t_fat* crear_fat_mapeada(char* path)
{
    t_fat* fat = malloc(sizeof(t_fat));
    fat->size = (cant_bloques_total - cant_bloques_swap) * sizeof(uint32_t);
    struct stat file_stat;
    fat->file_descriptor = open(path, O_RDWR);
    fstat(fat->file_descriptor, &file_stat);
    off_t file_size = file_stat.st_size;
    if(fat->file_descriptor == -1)
	{
		fat->file_descriptor = open(path, O_CREAT | O_RDWR, (mode_t) 0664);
        ftruncate(fat->file_descriptor, fat->size);
        fat->memory_map = mmap(NULL, fat->size, PROT_READ | PROT_WRITE, MAP_SHARED, fat->file_descriptor, 0);

        fat->memory_map[0] = UINT32_MAX;
        for(uint32_t i = 1; i < (cant_bloques_total- cant_bloques_swap) ; i++)
        {
            fat->memory_map[i] = 0;
        }

	}
    else
    {
        fat->memory_map = mmap(NULL, file_size, PROT_WRITE, MAP_PRIVATE, fat->file_descriptor, 0);
        if(file_size != fat->size)
        {
            ftruncate(fat->file_descriptor, fat->size);
            printf("ERROR: El tamaño de la fat es distinto al tamaño del archivo fat.dat\n");
            fat->memory_map[0] = UINT32_MAX;
            for(uint32_t i = 1; i < (cant_bloques_total- cant_bloques_swap) ; i++)
            {
                fat->memory_map[i] = 0;
            }
        }
    }

    //msync(fat->memory_map, size, MS_SYNC);
    //munmap(fat->memory_map, size);
    //close(fat->file_descriptor);
    return fat;
}

uint32_t obtener_bloque_libre()
{
    for(uint32_t i = 1; i < cant_bloques_total - cant_bloques_swap; i++)
    {
        if(fat->memory_map[i] == 0)
        {
            return i;
        }
    }
    return UINT32_MAX;
}
