#ifndef FAT_H
#define FAT_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <log/log_utils.h>
#include <pcb/pcb.h>
#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct
{
    int file_descriptor;
    uint32_t* memory_map;
}t_fat;

t_fat* crear_fat_mapeada(char* path, uint32_t size);
uint32_t obtener_bloque_libre();

#endif