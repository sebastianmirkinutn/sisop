#include "main.h"

/*
int32_t abrir_archivo(char* path_fcb, char* nombre)
{
	char* ruta = malloc(strlen(path_fcb) + 1 + strlen(nombre) + 4 + 1);
	strcpy(ruta, path_fcb);
	uint32_t tam_archivo;
	strcat(ruta, "/");
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	int archivo_fcb = open(ruta, O_RDONLY);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    close(archivo_fcb);
    t_fcb* fcb = leer_fcb(path_fcb, nombre);
	tam_archivo = fcb->tam_archivo;
	liberar_fcb(fcb);
	printf("aa_tam_archivo = %i\n", tam_archivo);
    return tam_archivo;
}

uint32_t crear_archivo(char* path_fcb, char* nombre)
{
    t_fcb* fcb = malloc(sizeof(t_fcb));
	char* ruta = malloc(strlen(path_fcb) + 1 + strlen(nombre) + 4 + 1);
	strcpy(ruta, path_fcb);
	uint32_t tam_archivo;
	strcat(ruta, "/");
    strcat(ruta, nombre);
    strcat(ruta, ".fcb");
	printf("PATH: %s", ruta);
	int archivo_fcb = open(ruta, O_CREAT | O_RDWR, 0664);
	t_config* config_fcb = iniciar_config(ruta);
	if(archivo_fcb == -1)
	{
		return -1;
	}
	config_set_value(config_fcb, "NOMBRE_ARCHIVO", nombre);
	config_set_value(config_fcb, "TAMANIO_ARCHIVO", "0");
	config_set_value(config_fcb, "BLOQUE_INICIAL", "0");
	config_save(config_fcb);
	//El archivo no va estar realmente abierto (o podríamos dejarlo abierto...).
    close(archivo_fcb);
    return 1;
}
*/
/*
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

	printf("----------------------------------------------------------\n");
    filesystem=iniciarArchivoFilesystem(filesystem,path_bloques);
	printf ("---> Apertura filesystem: bloques.dat <ok>\n");
    filesystem=creacionFilesystem(filesystem,path_bloques); //(QUIRAR A FUTUTO)reset temporal de archivo bloques.dat
	printf ("---> Reset filesystem: bloques.dat <ok>\n");

	fat=iniciarArchivoFAT(fat,path_fat,ui32_max_entradas_fat);
	printf ("---> Apertura de archivo fat.dat: <ok>\n");
	fat=reiniciar_fat(fat,ui32_max_entradas_fat); //(QUIRAR A FUTUTO)Reset de fat.dat para probar la creación de la tabla
	printf ("---> Reset tabla FAT <ok>\n");

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
			printf("m_tam_archivo = %i\n", tam_archivo);
			send(socket_kernel, &tam_archivo, sizeof(int32_t), NULL);
			break;

		case CREAR_ARCHIVO:
			nombre_archivo = recibir_mensaje(socket_kernel);
			
			if(crear_archivo(path_fcb, nombre_archivo))
			{
				enviar_respuesta(socket_kernel, OK);
			}
			else
			{
				enviar_respuesta(socket_kernel, ERROR);
			}
			break;

		case LEER_ARCHIVO:
			break;
		default:
			liberar_conexion(socket_kernel);
			return;
			break;
		}
*/

int main(int argc, char* argv[]) {

	uint32_t ui32_tam_de_archivo=0;
    uint32_t ui32_max_entradas_fat=0;
    uint32_t ui32_cant_bloques_total=0;
	uint32_t ui32_cant_bloques_swap=0;
	uint32_t ui32_tam_bloque=0;
	uint32_t tam_archivo=0;

	//Usados para operaciones de lectura y escritura
	char nombreArchivo[128]="";
	char modoApertura;
	char respuesta_ok[3]="ok";
	uint32_t ui32_cantBytesTruncar;
	uint32_t ui32_cantBytes_a_leer;
	uint32_t ui32_cantBytes_a_escribir;
	uint32_t ui32_posicionPuntero;
	uint32_t ui32_direccionDeMemoria;
	uint32_t ui32_fin_op_lectura;
	uint32_t ui32_fin_op_escritura;


	char buffer_data[1024+1]="";   
	char valorParametro[128]="";
	

	//------------------------------------------------------------------------
    t_log* logger = iniciar_logger("log_filesystem.log","FILESYSTEM");
    t_config* config = iniciar_config("../filesystem/cfg/filesystem.config");
    
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

	//Borra los archivos fcb para realizar las pruebas
	printf("----------------------------------------------------------\n");
	printf(">>Se borran los archivos .fcb creados previamente\n");
	printf("----------------------------------------------------------\n");
	remove("../filesystem/fcbs/documento1.fcb");
	remove("../filesystem/fcbs/documento2.fcb");
	remove("../filesystem/fcbs/documento3.fcb");
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
    while(1)  {
		printf("----------------------------------------------\n");
		operacion = recibir_operacion(socket_kernel);
		printf (">>Se recibe operacion:%d\n",operacion);

		switch (operacion) {
		 	case ABRIR_ARCHIVO: {
				printf(">>Operacion recibida es F_OPEN\n");
				//-------------------------------------------------------
				//--- Recive nombre de archivo a abrir
				recv(socket_kernel, &nombreArchivo, sizeof(nombreArchivo), NULL);
				printf (">Recibe: Nombre del archivo a abrir:%s\n",nombreArchivo);
				//-------------------------------------------------------
				//--- Recibe modo de apertura
				recv(socket_kernel, &modoApertura, sizeof(char), NULL);
				printf (">Recibe: Modo de apertura:%c\n",modoApertura);
				//--- Crea el archivo si no existe y sino lo abre informado en tamaño
				tam_archivo=(uint32_t) abrir_archivo(nombreArchivo,path_fcb,logger);
				send(socket_kernel, &tam_archivo, sizeof(uint32_t), NULL);
				if (tam_archivo==-1) printf(">Envía: El archivo: %s no existe\n",nombreArchivo);
				else printf(">Envía: El archivo existe, se abre en el modo:%c\n",modoApertura);
				break;
			}
			case CREAR_ARCHIVO: {
				printf(">>Operacion recibida es F_CREATE_FILE\n");
				//-------------------------------------------------------
				//--- Recibe nombre de archivo a crear
				recv(socket_kernel, &nombreArchivo, sizeof(nombreArchivo), NULL);
				printf (">Recibe: Nombre del archivo a abrir:%s\n",nombreArchivo);
				//-------------------------------------------------------
				//--- Recibe modo de apertura
				recv(socket_kernel, &modoApertura, sizeof(char), NULL);
				printf (">Recibe: Modo de apertura:%c\n",modoApertura);
				//--- Crea el archivo si no existe y sino lo abre informado en tamaño
				crear_archivo(nombreArchivo,path_fcb,logger);
				send(socket_kernel, &respuesta_ok, sizeof(respuesta_ok), NULL);
				break;
			}

			case TRUNCAR_ARCHIVO: {
				printf(">>Operacion recibida es F_TRUNCATE\n");
				//-------------------------------------------------------
				//--- Recibe nombre de archivo a aplicar truncado
				recv(socket_kernel, &nombreArchivo, sizeof(nombreArchivo), NULL);
				printf (">Recibe: Nombre del archivo a abrir:%s\n",nombreArchivo);
				//-------------------------------------------------------
				//--- Recibe cantidad de bytes de truncados
				recv(socket_kernel, &ui32_cantBytesTruncar, sizeof(uint32_t), NULL);
				printf (">Recibe: Cantidad de Bytes a <Truncar>:%d\n",ui32_cantBytesTruncar);
				//------------------------------------------------------
				truncar_archivo(nombreArchivo,ui32_cantBytesTruncar,logger,fat,ui32_tam_bloque,ui32_max_entradas_fat,path_fcb);
				//--- Envía mensaje de operacion finalizada
				send(socket_kernel, &respuesta_ok, sizeof(respuesta_ok), NULL);
				break;
			}

			case LEER_ARCHIVO: {
				printf(">>Operacion recibida es F_READ\n");
				//-------------------------------------------------------
				//--- Recibe nombre de archivo a leer
				recv(socket_kernel, &nombreArchivo, sizeof(nombreArchivo), NULL);
				printf (">Recibe: Nombre del archivo a leer:%s\n",nombreArchivo);
				//--- Recibe cantidad de bytes a leer
				recv(socket_kernel, &ui32_cantBytes_a_leer, sizeof(uint32_t), NULL);
				printf (">Recibe: Cantidad de Bytes a leer:%d\n",ui32_cantBytes_a_leer);
				//------------------------------------------------------
				//--- Recibe Posicion del puntero
				recv(socket_kernel, &ui32_posicionPuntero, sizeof(uint32_t), NULL);
				printf (">Recibe: Posición del puntero en archivo:%u\n",ui32_posicionPuntero);		
				//-------------------------------------------------------
				//--- Direccion de memoria a volvar los datos
				recv(socket_kernel, &ui32_direccionDeMemoria, sizeof(uint32_t), NULL);
				printf (">Recibe: Direccion de memoria:%u\n",ui32_direccionDeMemoria);	
				//-------------------------------------------------------
				//--- Datos que se lee desde el archivo
				printf("## Datos leeidos desde el archivo:%s\n",leer_archivo_bloque_n("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb,ui32_posicionPuntero,ui32_cantBytes_a_leer,buffer_data));
				//-------------------------------------------------------
				//--- Envía mensaje de operacion finalizada
				ui32_fin_op_lectura=1;
				send(socket_kernel, &ui32_fin_op_lectura, sizeof(uint32_t), NULL);
				printf (">Envía operacion de lectura realizada\n");
				break;
			}

			case ESCRIBIR_ARCHIVO: {
				printf(">>Operacion recibida es F_WRITE\n");
				//-------------------------------------------------------
				//--- Recibe nombre de archivo a escribir
				recv(socket_kernel, &nombreArchivo, sizeof(nombreArchivo), NULL);
				printf (">Recibe: Nombre del archivo a escribir:%s\n",nombreArchivo);
				//--- Recibe cantidad de bytes a leer
				recv(socket_kernel, &ui32_cantBytes_a_escribir, sizeof(uint32_t), NULL);
				printf (">Recibe: Cantidad de Bytes a escribir:%d\n",ui32_cantBytes_a_escribir);
				//------------------------------------------------------
				//--- Recibe Posicion del puntero
				recv(socket_kernel, &ui32_posicionPuntero, sizeof(uint32_t), NULL);
				printf (">Recibe: Posición del puntero en archivo:%u\n",ui32_posicionPuntero);		
				//-------------------------------------------------------
				//--- Direccion de memoria a volvar los datos
				recv(socket_kernel, &ui32_direccionDeMemoria, sizeof(uint32_t), NULL);
				printf (">Recibe: Direccion de memoria:%u\n",ui32_direccionDeMemoria);	
				//-------------------------------------------------------
				char buffer_data[1024+1]="";
				//-- Emulación de pedidos de datos a la memoria para escribir en el archivo
				char paginaDeMemoria[1024+1]="";
				solicitarPaginaMemoria(paginaDeMemoria);
				//printf ("\n## El contenido de la página de memoria es:%s\n",paginaDeMemoria);
				determinaDatosEnPagina(paginaDeMemoria,ui32_direccionDeMemoria,ui32_cantBytes_a_escribir,buffer_data);
				printf ("\n## Datos tomados de la direccion de memoria que se almacenar en el archivo son:%s\n",buffer_data);
				escribir_archivo_n("documento1",logger,fat,ui32_tam_bloque,filesystem,path_fcb,ui32_posicionPuntero,ui32_cantBytes_a_escribir,buffer_data);
				//-------------------------------------------------------
				//--- Envía mensaje de operarcion finalizada
				ui32_fin_op_escritura=1;
				send(socket_kernel, &ui32_fin_op_escritura, sizeof(uint32_t), NULL);
				printf (">Envía: operacion de escritura realizada\n");
				break;
			}
			case RESET_FILE_SYSTEM: {
				creacionFilesystem(filesystem,path_bloques);
			}

			case RESET_FAT: {
				reiniciar_fat(fat,ui32_max_entradas_fat);
				break;
			}

			case MOSTRAR_TABLA_FAT: {
				mostrar_tabla_FAT(fat,ui32_max_entradas_fat);
				break;
			}

			case FIN_DE_PROGRAMA: {
				fclose(fat);
				fclose(filesystem);
				liberar_conexion(socket_kernel);
				return EXIT_SUCCESS;
				break;
			}
			default: {
				//Completar con...
			}
		}
    }
    return 0;
}