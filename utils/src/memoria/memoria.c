#include "memoria.h"

void pedir_frame(int socket)
{
    op_code operacion = PEDIDO_DE_FRAME;
    send(socket, &operacion, sizeof(op_code), NULL);
}

uint32_t recibir_frame(int socket)
{
    op_code operacion;
    uint32_t frame;
    recv(socket, &operacion, sizeof(op_code), MSG_WAITALL);
    switch (operacion)
    {
    case FRAME:
        recv(socket, &frame, sizeof(op_code), MSG_WAITALL);
        break;
    
    default:
        break;
    }
    return frame;
}

void enviar_frame (int socket, uint32_t frame)
{
    op_code operacion = FRAME;
    send(socket, &operacion, sizeof(op_code), NULL);
}