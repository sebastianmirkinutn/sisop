# include "pcb.h"

uint32_t contador_pid = 1;

t_registros* crear_registros()
{
    t_registros* registros = malloc(sizeof(t_registros));
    registros->AX = 0;
    registros->BX = 0;
    registros->CX = 0;
    registros->DX = 0;
    registros->PC = 0;
    return registros;
}

t_pcb* crear_pcb(uint32_t prioridad, char* pseudocodigo)
{
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->prioridad = prioridad;
    pcb->estado = NEW;
    pcb->pid = contador_pid;
    pcb->contexto = crear_registros();
    pcb->tabla_de_archivos_abiertos = list_create();
    pcb->archivo_de_pseudocodigo = strndup(pseudocodigo, strlen(pseudocodigo) + 1);
    pcb->recursos_asignados = list_create();
    contador_pid++;
    return pcb;
}

void liberar_pcb(t_pcb* pcb)
{
    list_destroy_and_destroy_elements(pcb->tabla_de_archivos_abiertos, free);
    free(pcb->contexto);
    free(pcb);
}

void* serializar_contexto(t_registros* registros)
{
	void * magic = malloc(sizeof(uint32_t) * 5);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(registros->AX), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
    printf("AX = %i\n",registros->AX);
	memcpy(magic + desplazamiento, &(registros->BX), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, &(registros->CX), sizeof(uint32_t));
    desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, &(registros->DX), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
    memcpy(magic + desplazamiento, &(registros->PC), sizeof(uint32_t));

	return magic;
}

t_registros* deserializar_contexto(void* magic)
{
	t_registros* registros = malloc(sizeof(t_registros));
	int desplazamiento = 0;

	memcpy(&(registros->AX), magic + desplazamiento, sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
    printf("AX = %i\n", registros->AX);
	memcpy(&(registros->BX), magic + desplazamiento, sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(&(registros->CX), magic + desplazamiento, sizeof(uint32_t));
    desplazamiento+= sizeof(uint32_t);
	memcpy(&(registros->DX), magic + desplazamiento, sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
    memcpy(&(registros->PC), magic + desplazamiento, sizeof(uint32_t));

	return registros;
}

/*
t_registros* recibir_contexto_de_ejecucion(int socket)
{
	op_code operacion;
    int size;
    t_registros* registros;
    void* recibido;
	recv(socket, &operacion, sizeof(op_code), MSG_WAITALL);
    recv(socket, &size, sizeof(int), MSG_WAITALL);
    recv(socket, recibido, size, MSG_WAITALL);
    registros = deserializar_contexto(recibido);
    return registros;
}

void enviar_contexto_de_ejecucion(t_registros* registros, int socket)
{
    op_code operacion = PAQUETE;
    void* a_enviar = serializar_contexto(registros);
    int size = sizeof(uint32_t) * 5;
    send(socket, &operacion, sizeof(op_code), 0);
    send(socket, &size, sizeof(int), 0);
    send(socket, a_enviar, size, 0);
}
*/

t_registros* recibir_contexto_de_ejecucion(int socket)
{
    t_registros* registros = malloc(sizeof(t_registros));
	recv(socket, &(registros->BX), sizeof(uint32_t), MSG_WAITALL);
    recv(socket, &(registros->AX), sizeof(uint32_t), MSG_WAITALL);
    recv(socket, &(registros->CX), sizeof(uint32_t), MSG_WAITALL);
    recv(socket, &(registros->DX), sizeof(uint32_t), MSG_WAITALL);
    recv(socket, &(registros->PC), sizeof(uint32_t), MSG_WAITALL);
    return registros;
}

void enviar_contexto_de_ejecucion(t_registros* registros, int socket)
{
    send(socket, &(registros->AX), sizeof(uint32_t), 0);
    send(socket, &(registros->BX), sizeof(uint32_t), 0);
    send(socket, &(registros->CX), sizeof(uint32_t), 0);
    send(socket, &(registros->DX), sizeof(uint32_t), 0);
    send(socket, &(registros->PC), sizeof(uint32_t), 0);
}

t_motivo_desalojo recibir_motivo_desalojo(int socket){
    t_motivo_desalojo motivo;
	if(recv(socket, &motivo, sizeof(t_motivo_desalojo), MSG_WAITALL) > 0)
		return motivo;
	else
	{
		close(socket);
		return -1;
	}
}

void enviar_motivo_desalojo(int socket, t_motivo_desalojo motivo)
{
    send(socket, &motivo, sizeof(t_motivo_desalojo), 0);
}


t_recurso* crear_recurso(char* nombre, uint32_t instancias)
{
    t_recurso* recurso = malloc(sizeof(t_recurso));
    recurso->nombre = nombre;                   /*Habría que usar strdup(). Además, de esta forma*/
    recurso->instancias = instancias;           /*podemos liberar la lista de char* iniciar, a la */
    recurso->cola_blocked = queue_create();     /*que apuntan los punteros.*/
    sem_init(&(recurso->mutex_cola_blocked), 0, 1);
    return recurso;
}