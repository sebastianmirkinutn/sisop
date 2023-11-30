#ifndef OPERACIONES_H
#define OPERACIONES_H

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
#include <string.h>
#include <commons/memory.h>

#include "fcb.h"
#include "fat.h"
#include "utils_adicionales.h"

int32_t abrir_archivo(char* path_fcb, char* nombre);
uint32_t crear_archivo(char* path_fcb, char* nombre);
int32_t agrandar_archivo(t_fcb* archivo, uint32_t size);
int32_t truncar_archivo(char* nombre, uint32_t size);

#endif