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
				printf ("\nFIN DEL PROGRAMA\n\n");
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

