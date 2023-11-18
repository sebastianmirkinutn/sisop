#include "temp.h"


char secuencia[50][100]={
	"RESET_FILE_SYSTEM",
	"RESET_FAT",
	"F_OPEN documento1 W",
	"F_TRUNCATE documento1 100",
	"F_SEEK documento1 20",
	"F_WRITE documento1 14",
	"F_CLOSE documento1",
	"MOSTRAR_TABLA_FAT",
	"F_OPEN documento1 R",
	"F_SEEK documento1 20",
	"F_READ documento1 14",
	"F_CLOSE documento1",
	"F_OPEN documento2 W",
	"F_TRUNCATE documento2 3000",
	"F_SEEK documento2 100",
	"F_WRITE documento2 14",
	"F_CLOSE documento2",
	"MOSTRAR_TABLA_FAT",
	"F_OPEN documento2 R",
	"F_SEEK documento2 100",
	"F_READ documento2 14",
	"F_CLOSE documento2",
	"F_OPEN documento3 W",
	"F_TRUNCATE documento3 5000",
	"F_SEEK documento3 2100",
	"F_WRITE documento3 14",
	"F_CLOSE documento3",
	"MOSTRAR_TABLA_FAT",
	"F_OPEN documento3 R",
	"F_SEEK documento3 2100",
	"F_READ documento3 14",
	"F_CLOSE documento3",
	"FIN_DE_PROGRAMA",
};

void ejecutarSecuencia(int conexion) {
	op_code operacion;
	char nombreArchivo[128]="";
	char modoApertura;
	char c_cantBytesTruncar[10]="";
	char respuesta_ok[3]="";
	char c_cant_datos_a_leer[10];
	char c_cant_datos_a_escribir[10];
	char c_puntero_posic_archivo[10]="";

	uint32_t ui32_cantBytesTruncar;
	uint32_t ui32_tamanioArchivo;
	uint32_t ui32_cant_datos_a_leer;
	uint32_t ui32_cant_datos_a_escribir;
	uint32_t ui32_puntero_posic_archivo;
	uint32_t ui32_direccion_de_memoria;
	uint32_t ui32_respuesta_operacion_lectura;
	uint32_t ui32_respuesta_operacion_escritura;


    int opc=0;
	char instruccion[128]="";
	
	char parametro[128]="";
	char parametro_a_enviar[128]="";
	char *valor;

	uint32_t respuestaValor;
	int i;
	int index=0;
	int contEspacios=0;

	for (i=0;i<33;i++){
		printf("\n-----------------------------------------\n");
		printf(">>Ejecuta:%s\n",secuencia[i]);
		strcpy(instruccion,secuencia[i]);

		if (strncmp(instruccion,"F_OPEN",6)==0){
			//-------------------------------------------------
			//--Envía operacion
			operacion = ABRIR_ARCHIVO;
			enviar_operacion(conexion,operacion);
			//-------------------------------------------------
			//--Envía el nombre de archivo a abrir
			buscarDatoPorPosicion(&instruccion[7],nombreArchivo,1);
			printf(">Envía: Solicitud para abrir archivo:%s\n",nombreArchivo);
			send(conexion, &nombreArchivo, sizeof(nombreArchivo), NULL);
			//-------------------------------------------------
			//--Envía el modo de apertura de arhchivo
			modoApertura=instruccion[strlen(instruccion)-1];
			printf(">Envía: Modo apertura:%c\n",modoApertura);
			send(conexion, &modoApertura, sizeof(char), NULL);
			//-------------------------------------------------
			//--Recibe verificación de operacion
			recv(conexion, &ui32_tamanioArchivo, sizeof(int32_t), NULL);
			if (ui32_tamanioArchivo==-1) {
				printf(">Recibe: El archivo solicitado no existe. Se debe crear\n");
				printf("\n-----------------------------------------\n");
				operacion = CREAR_ARCHIVO;
				printf(">>Envía: solitud para crear archivo\n");
				enviar_operacion(conexion,operacion);
				//-------------------------------------------------
				//--Envía el nombre de archivo a crear
				printf(">Envía: Nombre del archivo a crear:%s\n",nombreArchivo);
				send(conexion, &nombreArchivo, sizeof(nombreArchivo), NULL);
				//-------------------------------------------------
				//--Envía el modo de apertura de arhchivo
				printf(">Envía: Modo apertura:%c\n",modoApertura);
				send(conexion, &modoApertura, sizeof(modoApertura), NULL);
				//--Recibe verificación <ok> de la operacion
				recv(conexion, &respuesta_ok, sizeof(respuesta_ok), NULL);
				if (!(strcmp("ok",respuesta_ok))) printf(">Recibe: El archivo %s fue creado correctamente\n",nombreArchivo);
				else printf("Recibe: No se pudo crear el archivo %s en Filesystem\n",nombreArchivo);
				}
			else printf(">Respuesta: El archivo existe y su tamaño es:%u Bytes\n",ui32_tamanioArchivo);
		}
		if (strncmp(instruccion,"F_SEEK",6)==0){
			//-- Obtiene el puntero que ubica la posición requerida dentro del archivo
			buscarDatoPorPosicion(&instruccion[7],c_puntero_posic_archivo,2);
			ui32_puntero_posic_archivo = (uint32_t) atoi(c_puntero_posic_archivo);
			printf(">>Posicion de puntero requerida:%u\n",ui32_puntero_posic_archivo);
		}
		else if (strncmp(instruccion,"F_READ",6)==0){
			operacion = LEER_ARCHIVO;
			enviar_operacion(conexion,operacion);
			//--Envía el nombre de archivo a leer
			buscarDatoPorPosicion(&instruccion[7],nombreArchivo,1);
			printf(">>Envía: Solicitud para Leer datos del archivo:%s\n",nombreArchivo);
			send(conexion, &nombreArchivo, sizeof(nombreArchivo), NULL);
			//-------------------------------------------------
			//--Envía la cantidad de datos a leer del archivo
			buscarDatoPorPosicion(&instruccion[7],c_cant_datos_a_leer,2);
			ui32_cant_datos_a_leer=(uint32_t) atoi(c_cant_datos_a_leer);
			printf(">Envía: Cantidad de bytes a leer:%u\n",ui32_cant_datos_a_leer);
			send(conexion, &ui32_cant_datos_a_leer, sizeof(uint32_t), NULL);
			//-------------------------------------------------
			//--Envía la posición de puntero del archivo
			send(conexion, &ui32_puntero_posic_archivo, sizeof(uint32_t), NULL);
			printf(">Envía: Posicion de puntero requerida :%u\n",ui32_puntero_posic_archivo);
			//-------------------------------------------------
			ui32_direccion_de_memoria=12345;
			send(conexion, &ui32_direccion_de_memoria, sizeof(uint32_t), NULL);
			printf(">Envía: dirección de memoria donde ubicar la lectura de datos :%u\n",ui32_direccion_de_memoria);
			//-------------------------------------------------
			//--Recibe verificación de operacion
			recv(conexion, &ui32_respuesta_operacion_lectura, sizeof(uint32_t), NULL);
			if (ui32_respuesta_operacion_lectura) printf("F_READ - Respuesta: Verificador de op CORRECTO\n");
			else printf("F_READ - Respuesta: Verificador de op INCORRECTO\n");
		}
		else if (strncmp(instruccion,"F_WRITE",7)==0){
			operacion = ESCRIBIR_ARCHIVO;
			enviar_operacion(conexion,operacion);
			//-------------------------------------------------
			//--Envía el nombre de archivo a escribir
			buscarDatoPorPosicion(&instruccion[8],nombreArchivo,1);
			printf(">>Se requiere escribir datos en el archivo:%s\n",nombreArchivo);
			send(conexion, &nombreArchivo, sizeof(nombreArchivo), NULL);
			//-------------------------------------------------
			//--Envía la cantidad de datos a escribir en el archivo
			buscarDatoPorPosicion(&instruccion[8],c_cant_datos_a_escribir,2);
			ui32_cant_datos_a_escribir=(uint32_t) atoi(c_cant_datos_a_escribir);
			printf(">Envía: Cantidad de bytes a escribir:%u\n",ui32_cant_datos_a_escribir);
			send(conexion, &ui32_cant_datos_a_escribir, sizeof(uint32_t), NULL);
			//-------------------------------------------------
			//--Envía la posición de puntero del archivo
			send(conexion, &ui32_puntero_posic_archivo, sizeof(uint32_t), NULL);
			printf(">Envía: Posicion de puntero requerida :%u\n",ui32_puntero_posic_archivo);
			//-------------------------------------------------
			ui32_direccion_de_memoria=20;
			send(conexion, &ui32_direccion_de_memoria, sizeof(uint32_t), NULL);
			printf(">Envía: dirección de memoria desde donde se tomaran los data a escribir:%u\n",ui32_direccion_de_memoria);
			//-------------------------------------------------
			//--Recibe verificación de operacion
			recv(conexion, &ui32_respuesta_operacion_escritura, sizeof(uint32_t), NULL);
			if (ui32_respuesta_operacion_escritura==1) printf("F_WRITE - Respuesta: Verificador de op CORRECTO\n");
			else printf("F_WRITE - Respuesta: Verificador de op INCORRECTO\n");
		}
		else if (strncmp(instruccion,"F_TRUNCATE",10)==0){
			//-------------------------------------------------
			//--Envía operacion
			operacion = TRUNCAR_ARCHIVO;
			enviar_operacion(conexion,operacion);
			//-------------------------------------------------
			//--Envía el nombre de archivo a truncar
			buscarDatoPorPosicion(&instruccion[11],nombreArchivo,1);
			printf(">Envía: Archivo a truncar:%s\n",nombreArchivo);
			send(conexion, &nombreArchivo, sizeof(nombreArchivo), NULL);
			//--Obtiene de la instruccion la cantidad de bytes del truncado y lo envía
			buscarDatoPorPosicion(&instruccion[11],c_cantBytesTruncar,2);
			ui32_cantBytesTruncar=(uint32_t) atoi(c_cantBytesTruncar);
			printf(">Envía: Tamanio de truncado:%u\n",ui32_cantBytesTruncar);
			send(conexion, &ui32_cantBytesTruncar, sizeof(uint32_t), NULL);
			//-------------------------------------------------
			//--Recibe verificación de operacion
			recv(conexion, &respuesta_ok, sizeof(respuesta_ok), NULL);
			if (!(strcmp("ok",respuesta_ok))) printf("Recibe: Operacion <truncado> realizada\n");
			else printf(">Recibe: Operacion <truncado> no se pudo realizar\n");
		}
		/*-----------------------------------------------------------------*/
		/*------ INSTRUCCIONES PARA OPERAR FILESYSTEM (TEMPORALES)---------*/
		else if (strncmp(instruccion,"RESET_FILE_SYSTEM",17)==0){				
            operacion = RESET_FILE_SYSTEM;
			printf("Envía: Reset de FILE_SYSTEM\n");
			enviar_operacion(conexion,operacion);
		}
		else if (strncmp(instruccion,"RESET_FAT",9)==0){				
            operacion = RESET_FAT;
			printf("Envía: Reset de FAT\n");
			enviar_operacion(conexion,operacion);
		}
		else if (strncmp(instruccion,"MOSTRAR_TABLA_FAT",17)==0){				
            operacion = MOSTRAR_TABLA_FAT;
			printf("Envía: MOSTRAR TABLA FAT\n");
			enviar_operacion(conexion,operacion);
		}
		else if (strncmp(instruccion,"FIN_DE_PROGRAMA",17)==0){		
			printf("Envía: FIN DE PROGRAMA\n");		
            operacion = FIN_DE_PROGRAMA;
			enviar_operacion(conexion,operacion);
		}
	}
	printf(">>> Fin ejecucion instrucciones FILESYSTEM\n\n");
}
