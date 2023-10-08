# include "pcb.h"

uint32_t contador_pid;

t_list* parsear_instrucciones(char* str)
{
    if(str == NULL)
    {
       return NULL;
    }
    t_list* instrucciones = list_create();
    char* str_cpy = strdup(str);
    char* token; strtok(str_cpy, "\n");
    while(token != NULL)
    {
        if (strlen(token) > 0)
        {
            char* token_cpy = strdup(token);
            token_cpy[strlen(token_cpy)] = '\0';
            list_add(instrucciones, token_cpy);
        }
        token = strtok(NULL, "\n");
    }
    return instrucciones;
}

t_pcb* crear_pcb(uint32_t prioridad)
{
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->prioridad = prioridad;
    pcb->estado = NEW;
    pcb->pid = contador_pid;
    contador_pid++;
    pcb->contexto = malloc(sizeof(t_registros));
    pcb->tabla_de_archivos_abiertos = list_create();
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
	void * magic = malloc(sizeof(uint32_t) * 4);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(registros->AX), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, &(registros->BX), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, &(registros->CX), sizeof(uint32_t));
    desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, &(registros->DX), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);

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

	return registros;
}

t_registros* recibir_contexto_de_ejecucion(int socket)
{
	int cod_op, size;
    t_registros* registros;
    void* recibido;
	if(recv(socket, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		if(cod_op == PAQUETE)
        {
            recv(socket, &size, sizeof(int), MSG_WAITALL);
            recv(socket, &recibido, size, MSG_WAITALL);
            registros = deserializar_contexto(recibido);
            return registros;
        }
        else
        {
            //error
        }
	else
	{
        //error
		close(socket);
	}
}

void enviar_contexto(t_registros* registros, int socket)
{
    op_code operacion = PAQUETE;
    void* a_enviar = serializar_contexto(registros);
    int size = sizeof(uint32_t) * 4;
    send(socket, &operacion, sizeof(operacion), 0);
    send(socket, &size, sizeof(int), 0);
    send(socket, &a_enviar,sizeof(uint32_t) * 4, 0);
}