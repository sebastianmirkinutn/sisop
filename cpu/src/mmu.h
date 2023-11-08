#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <math.h>
#include <memoria/memoria.h>
#include <stdio.h>
#include <stdlib.h>

t_direccion_fisica* traducir_direccion(char* direccion_logica, uint32_t tam_pagina, int socket_memoria);

#endif