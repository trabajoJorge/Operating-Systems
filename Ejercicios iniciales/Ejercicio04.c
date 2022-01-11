#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void main (int argc, char *argv[]) {
    int n1= atoi(argv[1]);
    char *a= (char *)malloc (n1*sizeof(char));
    for(int i = 0; i < n1; i++) {
        a[i]= 'a' + rand() % 26;
        printf("%c ", a[i]);
    }
}


 