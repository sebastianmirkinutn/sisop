#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
 
char *itoa(uint32_t dividendo,char *cadenaText) {
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
