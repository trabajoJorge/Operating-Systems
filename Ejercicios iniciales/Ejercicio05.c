#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char * crearArray(int n){
    char *a= (char *)malloc (n*sizeof(char));
    return a;
}

char caracterAleatorio(){
    return 'a' + rand() % 26;
}

void main (int argc, char *argv[]) {
    int n1= atoi(argv[1]);
    char *a= crearArray(n1);
    for(int i = 0; i < n1; i++) {
        a[i]= caracterAleatorio();
        printf("%c ", a[i]);
    }
}

