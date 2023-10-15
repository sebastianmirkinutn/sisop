# include "pcb.h"

uint32_t contador_pid;

t_pcb* crear_pcb(uint32_t prioridad, char* pseudocodigo)
{
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->prioridad = prioridad;
    pcb->estado = NEW;
    pcb->pid = contador_pid;
    contador_pid++;
    pcb->contexto = malloc(sizeof(t_registros));
    pcb->tabla_de_archivos_abiertos = list_create();
    pcb->archivo_de_pseudocodigo = strndup(pseudocodigo, strlen(pseudocodigo) + 1);
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
	memcpy(&(registros->BX), magic + desplazamiento, sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(&(registros->CX), magic + desplazamiento, sizeof(uint32_t));
    desplazamiento+= sizeof(uint32_t);
	memcpy(&(registros->DX), magic + desplazamiento, sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
    memcpy(&(registros->PC), magic + desplazamiento, sizeof(uint32_t));

	return registros;
}

t_registros* recibir_contexto_de_ejecucion(int socket)
{
    printf("recibir_contexto_de_ejecucion - socket: %i\n", socket);
	op_code cod_op;
    int size;
    t_registros* registros = malloc(sizeof(t_registros));
    void* recibido;
	if(recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL) > 0)
		if(cod_op == PAQUETE)
        {
            printf("cod_op: %i", cod_op);
            recv(socket, &size, sizeof(int), MSG_WAITALL);
            printf("size: %i", size);
            recibido = malloc(size);
            recv(socket, &recibido, size, MSG_WAITALL);
            printf("recibido: %i", recibido);
            registros = deserializar_contexto(recibido);
            free(recibido);
            return registros;
        }
        else
        {
            printf("cod_op: %i", cod_op);
            //error
        }
	else
	{
        //error
        printf("Error al recibir el mensaje\n");
		close(socket);
	}
    printf("Salgo de recibir");
}

void enviar_contexto(t_registros* registros, int socket)
{
    op_code operacion = PAQUETE;
    printf("enviar_contexto - operacion=PAQUETE");
    void* a_enviar = serializar_contexto(registros);
    printf("ya serialice los registros", operacion);
    int size = sizeof(uint32_t) * 5;
    send(socket, &operacion, sizeof(operacion), 0);
    printf("enviar_contexto - cod_op: %i", operacion);
    send(socket, &size, sizeof(int), 0);
    printf("enviar_contexto - size: %i", size);
    send(socket, &a_enviar,sizeof(uint32_t) * 4, 0);
    printf("enviar_contexto - %i: %i", a_enviar);
}

t_motivo_desalojo recibir_desalojo(int socket){
    t_motivo_desalojo motivo;
	if(recv(socket, &motivo, sizeof(t_motivo_desalojo), MSG_WAITALL) > 0)
		return motivo;
	else
	{
		close(socket);
		return -1;
	}
}

void enviar_desalojo(int socket, t_motivo_desalojo motivo)
{
    send(socket, &motivo, sizeof(t_motivo_desalojo), 0);
}