#include "utils_adicionales.h"

int strlen_(char *texto){
	int i=0;
	while(texto[i++]!='\0');
	return(i-1);
}

uint32_t potencia(uint32_t num1,uint32_t num2) {
    uint32_t resultPotencia=1;
    uint32_t i=0;
    for (i=0;i<num2;i++) {
        resultPotencia=resultPotencia*num1;
    }
    return(resultPotencia);
}

uint32_t atoi_(char *c_cadenaNumero){
	uint32_t ui32_numeroEntero=0;
	uint32_t i_lenCadenaNumero;
	uint32_t i;
    uint32_t i_indice;

	i_lenCadenaNumero=strlen(c_cadenaNumero);
    printf("La longitud de la cadena es:%d\n",i_lenCadenaNumero);
	for (i=0;i<i_lenCadenaNumero;i++) {
        i_indice=i_lenCadenaNumero-i-1;
        ui32_numeroEntero=ui32_numeroEntero+(c_cadenaNumero[i_indice]-48)*potencia(10,i);
	}
	return (ui32_numeroEntero);
}


// Convierte un número de tipo de dato número a una cadena de caracteres
char *itoa_(uint32_t dividendo,char *cadenaText) {
    uint32_t divisor=10;
    uint32_t resto=0;
    uint32_t cadenaInt[10];
    int i=0;
    int max_numeros;

    while (dividendo>=divisor) {
        resto=dividendo % divisor;
        cadenaInt[i++]=resto;
        dividendo=dividendo/divisor;
    }
    cadenaInt[i++]=dividendo;
    max_numeros=i;

    for (i=max_numeros-1;i>=0;i--) {
        if (cadenaInt[i]==0) strcat(cadenaText,"0");
        if (cadenaInt[i]==1) strcat(cadenaText,"1");
        if (cadenaInt[i]==2) strcat(cadenaText,"2");
        if (cadenaInt[i]==3) strcat(cadenaText,"3");
        if (cadenaInt[i]==4) strcat(cadenaText,"4");
        if (cadenaInt[i]==5) strcat(cadenaText,"5");
        if (cadenaInt[i]==6) strcat(cadenaText,"6");
        if (cadenaInt[i]==7) strcat(cadenaText,"7");
        if (cadenaInt[i]==8) strcat(cadenaText,"8");
        if (cadenaInt[i]==9) strcat(cadenaText,"9");
    }
    return (cadenaText);
}


/*int main(void)
{
    char cadenaText[10]="";
    printf ("El numero convertido a texto es:%s\n",itoa(52,cadenaText));
}
*/
