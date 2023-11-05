#include "main.h"

uint32_t abrir_archivo(char* path_fcb, char* nombre)
{
    t_fcb* fcb = malloc(sizeof(t_fcb));
    char* ruta = path_fcb;
	uint32_t tam_archivo;
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	FILE* archivo_fcb = open(ruta, O_RDONLY);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	fseek(archivo_fcb, 0, SEEK_END);
	tam_archivo = ftell(archivo_fcb);
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    fclose(archivo_fcb);
    return tam_archivo;
}

int main(int argc, char* argv[]) {
    t_log* logger = iniciar_logger("log_filesystem.log","FILESYSTEM");
    t_config* config = iniciar_config("./cfg/filesystem.config");
    char* puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    char* ip_memoria = config_get_string_value(config,"IP_MEMORIA");
    char* puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	char* path_fcb = config_get_string_value(config,"PATH_FCB");

    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha);

    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);

    int socket_servidor = iniciar_servidor(logger, puerto_escucha);
    int socket_kernel = esperar_cliente(logger, socket_servidor);
    if(socket_kernel){
        log_info(logger,"Se conectó kernel");
    }
    op_code operacion;

    int32_t ui32_tam_de_archivo=0;
    uint32_t ui32_max_entradas_fat;
    uint32_t ui32_cant_bloques_total=0;
	uint32_t ui32_cant_bloques_swap;
	uint32_t ui32_tam_bloque;

    while(1)
    {
        operacion = recibir_operacion(socket_kernel);
        switch (operacion)
        {
		case F_OPEN:
			char* nombre = recibir_mensaje(socket_kernel);
			uint32_t tam_archivo = abrir_archivo(nombre, path_fcb);
			break;
		//----------------------------------------------//
        case 1:
			creacionFilesystem(filesystem,path_bloques);
			break;
		case 2:
			reiniciar_fat(fat,ui32_max_entradas_fat);
			break;
		case 3:
			abrir_archivo("documento1",path_fcb);
			break;
		case 4:
			crear_archivo("documento1",logger,path_fcb);
			break;
		case 5:
			truncar_archivo("documento1",ui32_tam_de_archivo,logger,fat,ui32_tam_bloque,ui32_max_entradas_fat,path_fcb);
			break;
		case 6:
			leer_archivo("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb);
			break;
		case 7:
			escribir_archivo("documento1",documentoArchivo,logger,fat,ui32_tam_bloque,filesystem,path_fcb);
			break;
		case 8:
			mostrar_tabla_FAT(fat,ui32_max_entradas_fat);
			break;
		case F_CLOSE:
		    fclose(fat);
		    fclose(filesystem);
		    return EXIT_SUCCESS;
			break;
		default:
			liberar_conexion(socket_kernel);
			return;
		break;
        }
    }
    return 0;
}