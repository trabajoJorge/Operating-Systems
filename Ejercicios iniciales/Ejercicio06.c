#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> //For syscalls
#include <signal.h>
#include <sys/types.h>
//#include <sys/wait.h> //For wait
#include <time.h> //for time

#define MSG_ERROR1 ""
#define MSG_ERROR2 ""
#define MSG_ERROR3 ""



int main(int argc, char *argv[]){
    // 1.  nb_tests  número de pruebas a realizar
    // 2.  min_time  tiempo mínimo de ejecución acordado (en segundos)
    // 3.  max_time  tiempo máximo de ejecución acordado (en segundos)
    // 4.  program [parameters] es el nombre (y los parámetros, si los hay) del programa cuyo tiempo de ejecución debe $    int nb_test = atoi(argv[1]);
    //int id, id1, id2, aux;
    //slong t, t1;
    int hijo_1, hijo_2, hijo_3;
    //int nb_test= atoi(argv[1]);

    //t=time(0);
    if ((hijo_1= fork()) == 0) {
      printf("I am the %d child", getpid(), getppid());
    }
    if ((hijo_2 = fork()) == 0) {
      printf("I am the %d child", getpid(), getppid());
    }
    if ((hijo_3 = fork()) == 0) {
      printf("I am the %d child", getpid(), getppid());
    }
}

