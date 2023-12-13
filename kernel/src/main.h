#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sockets/sockets.h>
#include <sockets/client_utils.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pcb/pcb.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <log/log_utils.h>
#include <pcb/pcb.h>
#include <pthread.h>

#include "temp.h"
#include "planificador_largo_plazo.h"
#include "planificador_mediano_plazo.h"
#include "planificador_corto_plazo.h"
#include "manejo_de_archivos.h"

void imprimir_procesos_por_estado();
t_queue* buscar_proceso_en_cola_bloqueados(t_list* recursos_disponibles, t_list* tabla_global_de_archivos, uint32_t pid);
void liberar_recursos_archivos(t_pcb* pcb, int socket_filesystem);
t_queue* obtener_queue(uint32_t pid);
void finalizar_proceso (uint32_t pid, int socket_cpu_dispatch, int socket_memoria);
void imprimir_recursos();

#endif 