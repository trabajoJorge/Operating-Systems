#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main (int argc, char *argv[]){
    int n1= atoi(argv[1]);
    int n2= atoi(argv[2]);
    
    printf("**** EJERCICIO 1 ****");
    
    char *error1 = "ERROR: Los parametros no son correctos.\n";
    char *error2 = "ERROR: Lo parametros no estan comprendidos en el rango definido.\n";
    
    if (argc!=3){
        fprintf(stderr,"%s %s", error1, argv[0]) ; 
        exit(1);
    }

    if (n1<1 || n1>4 || n2<1 || n2>4){
        fprintf(stderr,"%s %s", error2, argv[0]) ; 
        exit(1);
    }

    int rows= 1 << n1;
    int cols= 1 << n2;
    char *mat1 = (char *)malloc(rows * cols * sizeof(char));
    char *mat2 = (char *)malloc(rows * cols * sizeof(char));
    
    printf("\n");
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            //mat1[i, j]= 'a';  
            mat1[i, j]= 'a' + rand() % 26; //ESTO NO ME FUNCIONA
           //printf("%c ", mat1[i, j]);
        }
        printf("\n");
    }

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            printf("%c ", mat1[i, j]);
        }
        printf("\n");
    }

    printf("\n");
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            mat2[i, j]= mat1[i, j] -32;
            //printf("%c ", mat2[i, j]);
        }
        printf("\n");
    }

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            printf("%c ", mat2[i, j]);
        }
        printf("\n");
    }
}
