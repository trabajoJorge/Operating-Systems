#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main (int argc, char *argv[]){
    int n= atoi(argv[1]);
    
    printf("**** EJERCICIO 1 ****");
    
    char *error1 = "ERROR: Los parametros no son correctos.\n";
    char *error2 = "ERROR: Lo parametros no estan comprendidos en el rango definido.\n";
    
    if (argc!=2){
        fprintf(stderr,"%s %s", error1, argv[0]) ; 
        exit(1);
    }

    if (n<5 || n>10){
        fprintf(stderr,"%s %s", error2, argv[0]) ; 
        exit(1);
    }

    int N= 1 << n; 
    char *a= (char *)malloc (N*sizeof(char));
    char *b= (char *)malloc (N*sizeof(char));
    memset(a, 0, N*sizeof(char));
    memset(a, 0, N*sizeof(char));
    
    printf("\n [ ");
    for(int i = 0; i < N; i++) {
        a[i]= 'a' + rand() % 26;
        printf("%d ", a[i]);
    }
    printf(" ]");

    printf("\n [ ");
    for(int i = 0; i < N; i++) {
        b[i] = a[i] -32;
        printf("%c ", b[i]);
    }
    printf("]");  
}
