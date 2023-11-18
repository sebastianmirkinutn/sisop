# include "fix_operation.h"


//mensaje: mensaje con parametros concatenados
//valor: es el string en donde escribirá el valor del parámetro buscado
//parametro: es la posicion del parametro deseado dentro del mensaje
char *buscarDatoPorPosicion(char *mensaje,char *valor,int parametro) {
    int i=0;
    int j=0;
    int w=0;

    parametro--;
    for (w=0;w<parametro;w++) {
        //Busca un espacio luego de cada parametro
        while (mensaje[i]!=' ') i++;
        i++;
    }
    //Busca los - luego del nombre del archivo
    while ((mensaje[i]!=' ') && (mensaje[i]!='\0')) {
        valor[j]=mensaje[i];
        i++;
        j++;
    }
    valor[j]='\0';
    return(valor);
}