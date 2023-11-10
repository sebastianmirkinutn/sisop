#include "temp.h"

// - Directivas para el Filesystem ------
#define RESET_FILE_SYSTEM 5000
#define RESET_FAT 5001
#define ABRIR_ARCHIVO 5002
#define CREAR_ARCHIVO 5003
#define TRUNCAR_ARCHIVO 5004
#define LEER_ARCHIVO 5005
#define ESCRIBIR_ARCHIVO 5006
#define MOSTRAR_TABLA_FAT 5007
#define FIN_DEL_PROGRAMA 5008
//--------------------------------------
// - Directivas de instrucciones ------
#define F_OPEN 6000
#define F_CLOSE 6001
#define F_SEEK 6002
#define F_READ 6003
#define F_WRITE 6004
#define F_TRUNCATE 6005


/*----------- Añadido temporal para controlat el FILESYSTEM -----------*/
void operacionesFilesSystem(int conexion) {
    op_code operacion;
    int opc=0;
	while (opc!=9) {
		printf("\n--------------------------------------------------------------\n");
		printf ("--- Operaciones con filesyste v.Kernel---\n");
		printf ("1) Resetear Filesystem - setea todos los bloques de datos a 0\n");
		printf ("2) Resetear FAT - setea todas las entradas a 0\n");
		printf ("3) Abrir archivo\n");
		printf ("4) Crear archivo\n");
		printf ("5) Truncar archivo\n");
		printf ("6) Leer archivo\n");
		printf ("7) Escribir archivo\n");
		printf ("8) Mostrar tabla FAT\n");
		printf ("9) Salir - Terminar\n");
		printf ("** (Recordatorio):Secuencia normal:4 - 5 - 7 - 6 - Para crear/escribir/leer un archivo **\n");
		printf ("Ingrese la opcion-> \n");
		scanf("%d",&opc);
		switch (opc) {
			case 1:
				printf ("Opcion elegida - 1) RESET_FILESYSTEM\n");
                operacion = RESET_FILE_SYSTEM;
				enviar_operacion(conexion,operacion);
				break;
			case 2:
				printf ("Opcion elegida - 2) RESET_FAT\n");
                operacion = RESET_FAT;
				enviar_operacion(conexion,operacion);
				break;
			case 3:
				printf ("Opcion elegida - 3) ABRIR_ARCHIVO\n");
                operacion = ABRIR_ARCHIVO;
				enviar_operacion(conexion,operacion);
				break;
			case 4:
				printf ("Opcion elegida - 4) CREAR_ARCHIVO\n");
                op_code operacion = CREAR_ARCHIVO;
				enviar_operacion(conexion,operacion);
				break;
			case 5:
				printf ("Opcion elegida - 5) TRUNCAR_ARCHIVO\n");
                operacion = TRUNCAR_ARCHIVO;
				enviar_operacion(conexion,operacion);
				break;
			case 6:
				printf ("Opcion elegida - 6) LEER_ARCHIVO\n");
                operacion = LEER_ARCHIVO;
				enviar_operacion(conexion,operacion);
				break;
			case 7:
				printf ("Opcion elegida - 7) ESCRIBIR ARCHIVO\n");
                operacion = ESCRIBIR_ARCHIVO;
				enviar_operacion(conexion,operacion);
				break;
			case 8:
				printf ("Opcion elegida - 8) MOSTRAR_TABLA_FAT\n");
                operacion = MOSTRAR_TABLA_FAT;
				enviar_operacion(conexion,operacion);
				break;
			case 9:
				printf ("\nFIN DEL EMULADOR FILESYSTEM\n\n");
                operacion = FIN_DEL_PROGRAMA;
				enviar_operacion(conexion,operacion);
				break;
			default:
				printf ("\n¡La opción ingresada no existe!\n\n");
		}
		printf("\n--------------------------------------------------------------\n");
	}
}
/*-------------------------------------------------------------------*/
/*----------- Añadido temporal para controlat el FILESYSTEM -----------*/
void operacionesInstrucciones(int conexion) {
    op_code operacion;
    int opc=0;
	while (opc!=7) {
		printf("\n--------------------------------------------------------------\n");
		printf ("--- Emulador de instrucciones de archivos ---\n");
		printf ("1) F_OPEN\n");
		printf ("2) F_CLOSE\n");
		printf ("3) F_SEEK\n");
		printf ("4) F_READ\n");
		printf ("5) F_WRITE\n");
		printf ("6) F_TRUNCATE\n");
		printf ("7) Salir - Terminar\n");
		printf ("Ingrese la opcion-> \n");
		scanf("%d",&opc);
		switch (opc) {
			case 1:
				printf ("Opcion elegida - 1) F_OPEN\n");
                operacion = F_OPEN;
				enviar_operacion(conexion,operacion);
				break;
			case 2:
				printf ("Opcion elegida - 2) F_CLOSE\n");
                operacion = F_CLOSE;
				enviar_operacion(conexion,operacion);
				break;
			case 3:
				printf ("Opcion elegida - 3) F_SEEK\n");
                operacion = F_SEEK;
				enviar_operacion(conexion,operacion);
				break;
			case 4:
				printf ("Opcion elegida - 4) F_READ\n");
                op_code operacion = F_READ;
				enviar_operacion(conexion,operacion);
				break;
			case 5:
				printf ("Opcion elegida - 5) F_WRITE\n");
                operacion = F_WRITE;
				enviar_operacion(conexion,operacion);
				break;
			case 6:
				printf ("Opcion elegida - 6) F_TRUNCATE\n");
                operacion = F_TRUNCATE;
				enviar_operacion(conexion,operacion);
				break;
			case 7:
				printf ("\nFIN DEL EMULADOR DE INSTRUCCIONES\n\n");
                operacion = FIN_DEL_PROGRAMA;
				enviar_operacion(conexion,operacion);
				break;
			default:
				printf ("\n¡La opción ingresada no existe!\n\n");
		}
		printf("\n--------------------------------------------------------------\n");
	}
}
