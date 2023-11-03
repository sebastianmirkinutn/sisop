#include "memoria.h"

void pedir_frame(int socket)
{
    op_code operacion = PEDIDO_DE_FRAME;
    send(socket, &operacion, sizeof(op_code), NULL);
}

void enviar_frame (int socket, int32_t frame)
{
    op_code operacion = FRAME;
    send(socket, &operacion, sizeof(op_code), NULL);
    send(socket, &frame, sizeof(int32_t), NULL);
}

int32_t recibir_frame(int socket)
{
    op_code operacion;
    int32_t frame;
    recv(socket, &operacion, sizeof(op_code), MSG_WAITALL);
    switch (operacion)
    {
    case FRAME:
        recv(socket, &frame, sizeof(int32_t), MSG_WAITALL);
        break;
    
    default:
        break;
    }
    return frame;
}