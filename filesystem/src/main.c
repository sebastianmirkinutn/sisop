#include "main.h"


int32_t abrir_archivo(char* path_fcb, char* nombre)
{
    char* ruta = path_fcb;
	uint32_t tam_archivo;
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	FILE* archivo_fcb = open(ruta, O_RDONLY);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    fclose(archivo_fcb);
    t_fcb* fcb = leer_fcb(path_fcb, nombre);
	tam_archivo = fcb->tam_archivo;
	liberar_fcb(fcb);
    return tam_archivo;
}

uint32_t crear_archivo(char* path_fcb, char* nombre)
{
    t_fcb* fcb = malloc(sizeof(t_fcb));
	char* ruta = path_fcb;
	uint32_t tam_archivo;
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	t_config* config_fcb = iniciar_config(ruta);
	FILE* archivo_fcb = open(ruta, O_CREAT);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	fseek(archivo_fcb, 0, SEEK_SET);
	config_set_value(config_fcb, "NOMBRE_ARCHIVO", nombre);
	config_set_value(config_fcb, "TAMANIO_ARCHIVO", nombre);
	config_set_value(config_fcb, "BLOQUE_INICIAL", nombre);
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    fclose(archivo_fcb);
    return 1;
}

int main(int argc, char* argv[]) {

	uint32_t ui32_tam_de_archivo=0;
    uint32_t ui32_max_entradas_fat=0;
    uint32_t ui32_cant_bloques_total=0;
	uint32_t ui32_cant_bloques_swap=0;
	uint32_t ui32_tam_bloque=0;

	//Usados para operaciones de lectura y escritura
	char buffer_data[1024+1]="";   
	char valorParametro[128]="";

	//------------------------------------------------------------------------
    t_log* logger = iniciar_logger("log_filesystem.log","FILESYSTEM");
    t_config* config = iniciar_config("./cfg/filesystem.config");
    
	char* puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    char* ip_memoria = config_get_string_value(config,"IP_MEMORIA");
    char* puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
	char* path_fat = config_get_string_value(config,"PATH_FAT");
    char* path_bloques = config_get_string_value(config,"PATH_BLOQUES");
	char* path_fcb = config_get_string_value(config,"PATH_FCB");
	char* cant_bloques_total=config_get_string_value(config,"CANT_BLOQUES_TOTAL");
    char* cant_bloques_swap=config_get_string_value(config,"CANT_BLOQUES_SWAP");
    char* tam_bloque=config_get_string_value(config,"TAM_BLOQUE");
    //char* retardo_acceso_bloque=config_get_string_value(config,"RETARDO_ACCESO_BLOQUE");
    //char* retardo_acceso_fat=config_get_string_value(config,"RETARDO_ACCESO_FAT");

    ui32_cant_bloques_total=atoi(cant_bloques_total);
    ui32_cant_bloques_swap=atoi(cant_bloques_swap);
    ui32_max_entradas_fat=ui32_cant_bloques_total-ui32_cant_bloques_swap;
    ui32_tam_bloque=atoi(tam_bloque);

    printf("PUERTO_ESCUCHA=%s\n",puerto_escucha);

    int conexion_memoria = crear_conexion(logger, ip_memoria, puerto_memoria);

    int socket_servidor = iniciar_servidor(logger, puerto_escucha);
    int socket_kernel = esperar_cliente(logger, socket_servidor);
    if(socket_kernel){
        log_info(logger,"Se conectó kernel");
    }
    //Creación e inicialización de Filesystem y Fat
	/*-------------------------------------------------------*/
	printf("----------------------------------------------------------\n");
    filesystem=iniciarArchivoFilesystem(filesystem,path_bloques);
	printf ("---> Apertura filesystem: bloques.dat <ok>\n");
    filesystem=creacionFilesystem(filesystem,path_bloques); //(QUIRAR A FUTUTO)reset temporal de archivo bloques.dat
	printf ("---> Reset filesystem: bloques.dat <ok>\n");
	/*-------------------------------------------------------*/
	fat=iniciarArchivoFAT(fat,path_fat,ui32_max_entradas_fat);
	printf ("---> Apertura de archivo fat.dat: <ok>\n");
	fat=reiniciar_fat(fat,ui32_max_entradas_fat); //(QUIRAR A FUTUTO)Reset de fat.dat para probar la creación de la tabla
	printf ("---> Reset tabla FAT <ok>\n");
	/*-------------------------------------------------------*/
	//Documento para probar el funcionamiento del Filesystem y Fat
	printf ("---> Emulación de contenido en memoria de: documento1.txt <ok>\n");
    ui32_tam_de_archivo=abrirDocumento("../filesystem/archivo_datos/documento1.txt",documentoArchivo);
	printf("----------------------------------------------------------\n");
    op_code operacion;
	t_list* lista;
	char* nombre_archivo;
	int32_t tam_archivo;
    while(1)
    {
		operacion = recibir_operacion(socket_kernel);
		printf ("\nSe recibe operacion:%d\n",operacion);
		switch (operacion)
		{
		case ABRIR_ARCHIVO:
			nombre_archivo = recibir_mensaje(socket_kernel);
			tam_archivo = abrir_archivo(path_fcb, nombre_archivo);
			send(socket_kernel, &tam_archivo, sizeof(int32_t), NULL);
			break;

		case CREAR_ARCHIVO:
			crear_archivo(path_fcb, nombre_archivo);
			break;
		
		default:
			liberar_conexion(socket_kernel);
			break;
		}


/*
		if (operacion<6000) {
			if (operacion==0) {
				//char* nombre = recibir_mensaje(socket_kernel);
				//uint32_t tam_archivo = abrir_archivo(nombre, path_fcb);
			}
			else if (operacion==RESET_FILE_SYSTEM){
				creacionFilesystem(filesystem,path_bloques);
			}
			else if (operacion==RESET_FAT){
				reiniciar_fat(fat,ui32_max_entradas_fat);
			}
			else if (operacion==ABRIR_ARCHIVO_D){
				abrir_archivo("../filesystem/archivo_datos/documento1",path_fcb);
			}
			else if (operacion==CREAR_ARCHIVO_D){
				crear_archivo_d("documento1",logger,path_fcb);
			}
			else if (operacion==TRUNCAR_ARCHIVO){
				truncar_archivo("documento1",ui32_tam_de_archivo,logger,fat,ui32_tam_bloque,ui32_max_entradas_fat,path_fcb);
			}
			else if (operacion==LEER_ARCHIVO){
				leer_archivo("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb);
			}
			else if (operacion==ESCRIBIR_ARCHIVO){
				escribir_archivo("documento1",documentoArchivo,logger,fat,ui32_tam_bloque,filesystem,path_fcb);
			}
			else if (operacion==MOSTRAR_TABLA_FAT){
				mostrar_tabla_FAT(fat,ui32_max_entradas_fat);
			}
			else if (operacion==FIN_DEL_PROGRAMA){
				fclose(fat);
				fclose(filesystem);
				return EXIT_SUCCESS;
			}
			else {
				liberar_conexion(socket_kernel);
				return;
			}
		}
		else {
			if (operacion==F_READ) {
				

				printf("La operacion recibida es F_READ\n");
				char *valor=recibir_mensaje(socket_kernel);
				printf("El valor recibido es:%s\n",valor);
				//-------------------------------------------------------
				//--- Nombre de archivo
				buscaDatoEnMensaje(valor,valorParametro,1);
				printf ("Nombre del archivo recibido:%s\n",valorParametro);
				//-------------------------------------------------------
				//--- Posicion del puntero
				uint32_t ui32_posicionPuntero=(uint32_t) atoi(buscaDatoEnMensaje(valor,valorParametro,2));
				printf ("Posición del puntero en archivo:%u\n",ui32_posicionPuntero);
				//-------------------------------------------------------
				//--- Cantidad de bytes a leer
				uint32_t ui32_cantBytes=(uint32_t) atoi(buscaDatoEnMensaje(valor,valorParametro,3));
				printf ("Cantidad d Bytes a leer:%u\n",ui32_cantBytes);				
				//-------------------------------------------------------
				//--- Direccion de memoria a volvar los datos
				uint32_t ui32_direccionDeMemoria=(uint32_t) atoi(buscaDatoEnMensaje(valor,valorParametro,4));
				printf ("Direccion de memoria:%u\n",ui32_direccionDeMemoria);	
				//-------------------------------------------------------
				//--- Direccion de memoria a volvar los datos
				printf("Datos del bloque:%s\n",leer_archivo_bloque_n("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb,ui32_posicionPuntero,ui32_cantBytes,buffer_data));
				//-------------------------------------------------------
				//--- Envía mensaje de operarcion finalizada
				enviar_mensaje("1",socket_kernel);
			}
			else if (operacion==F_WRITE) {
				char valorParametro[128]="";

				printf("La operacion recibida es F_WRITE\n");
				char *valor=recibir_mensaje(socket_kernel);
				printf("El valor recibido es:%s\n",valor);
				//-------------------------------------------------------
				//--- Nombre de archivo
				buscaDatoEnMensaje(valor,valorParametro,1);
				printf ("Nombre del archivo recibido:%s\n",valorParametro);
				//-------------------------------------------------------
				//--- Posicion del puntero
				uint32_t ui32_posicionPuntero=(uint32_t) atoi(buscaDatoEnMensaje(valor,valorParametro,2));
				printf ("Posición del puntero en archivo:%u\n",ui32_posicionPuntero);
				//-------------------------------------------------------
				//--- Cantidad de bytes a leer
				uint32_t ui32_cantBytes=(uint32_t) atoi(buscaDatoEnMensaje(valor,valorParametro,3));
				printf ("Cantidad d Bytes a leer:%u\n",ui32_cantBytes);				
				//-------------------------------------------------------
				//--- Direccion de memoria a volvar los datos
				uint32_t ui32_direccionDeMemoria=(uint32_t) atoi(buscaDatoEnMensaje(valor,valorParametro,4));
				printf ("Direccion de memoria:%u\n",ui32_direccionDeMemoria);	
				//-------------------------------------------------------
				//--- Direccion de memoria a volvar los datos
				char buffer_data[1024+1]="";
				//-- Emulación de pedidos de datos a la memoria para escribir en el archivo
				char paginaDeMemoria[1024+1]="";
				solicitarPaginaMemoria(paginaDeMemoria);
				printf ("El contenido de la página de memoria es:%s\n",paginaDeMemoria);
				determinaDatosEnPagina(paginaDeMemoria,ui32_direccionDeMemoria,ui32_cantBytes,buffer_data);
				printf ("Los datos a almacenar son:%s\n",buffer_data);
				escribir_archivo_n("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb,ui32_posicionPuntero,ui32_cantBytes,buffer_data);
				//-------------------------------------------------------
				//--- Envía mensaje de operarcion finalizada
				enviar_mensaje("1",socket_kernel);
			}
		}
		*/
    }
    return 0;
}