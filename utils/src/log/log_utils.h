#ifndef _LOG_UTILS_H_
#define _LOG_UTILS_H_

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>

t_log * iniciar_logger(char* archivo, char* nombre_del_proceso);
t_config* iniciar_config(char* path);

#endif