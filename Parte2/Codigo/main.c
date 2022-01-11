#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    
#include <time.h>   
#include "PCB.h"
#include "CPU.h"
#include "ColaProcesos.h"
#include "includes.h"
#include "defines.c"


colaPrioridades_T * colaPrioridades;
colaCPU_T * colaCPU;
CPU_T * cpu;

sem_t seg;
pthread_mutex_t mutexSeg, mutex_temp, mutexProcesos;

int temporizador;
int temporizadorPCB;
int procesoInterno;

void * generadorProcesos(void *id_gen) {
    int i;

    for (i=0; i<NUMPROCESOS; i++) {
        int prio = (rand()%NUMPRIORIDADES);
        int espera = (rand()%TIEMPOESPERA);
        int quantum = (rand()%QUANTUMMAX);
        int timepoVida = (rand()%TIEMPODEVIDA);
        PCB_T *pcb = crearPCB(i,timepoVida,prio,quantum);

        pthread_mutex_lock(&mutexProcesos); 
			encolarProceso(colaPrioridades, pcb); 
			procesoInterno=1;
        pthread_mutex_unlock(&mutexProcesos); 
		sleep(espera); 
    }
}

void * procesoPlanificador(void *s) {
    int i,j;
    while(1){ 
		pthread_mutex_lock(&mutexProcesos);	 
			if(procesoInterno==1){			
				for(i=0;i<cpu->nCores;i++){
					for(j=0;j<cpu->cores[i].nHilos;j++){				
						PCB_T *pcb =desencolarProceso(colaPrioridades);
						if(pcb!=NULL) addPCB_CPU(cpu, colaCPU, pcb);						
					}
				}
				procesoInterno=0;
			}
        pthread_mutex_unlock(&mutexProcesos);

		pthread_mutex_lock(&mutex_temp);
			if(temporizadorPCB==1){			
				for(i=0;i<cpu->nCores;i++){
					rmPCB_CPU(cpu, colaCPU, colaPrioridades);
					for(j=0;j<cpu->cores[i].nHilos;j++){				
						PCB_T *pcb = desencolarProceso(colaPrioridades);
						if(pcb!=NULL)addPCB_CPU(cpu, colaCPU, pcb);						
					}
				}
				temporizadorPCB=0;
			}
        pthread_mutex_unlock(&mutex_temp);
		
    } 
    
}

void *procesoReloj(void *id_clock) {   
	
    while(1) {  
		sleep(1);
        pthread_mutex_lock(&mutexSeg); 
			actualizarTiempo(cpu,colaCPU); 
			pthread_mutex_lock(&mutexProcesos); 
				procesoInterno = 1; 
			pthread_mutex_unlock(&mutexProcesos);
		pthread_mutex_unlock(&mutexSeg); 
		sem_post(&seg);
		mostrarProcesosCPU(cpu);
    }
}
void *procesoTemporizador(void *id_temp) {
    while(1) {
		//printf("Entra al while");
		sem_wait(&seg);

		pthread_mutex_lock(&mutexSeg); //lockeamos el mutex de seg
		temporizador++;

		if(temporizador==TIEMPO_ACTUALIZAR){
			temporizador=0;			 
			temporizadorPCB=1;		
			printf("\n Actualizado \n");	
		}
		pthread_mutex_unlock(&mutexSeg); //deslockeamos el mutex de seg
		

    }
}

int main( int argc, char *argv[]){
    if (argc!=3) {
        printf("ERROR: El numero de argumentos es incorrecto");
        exit(-1);
    }

    int NUMCPUS= 1;
    int NUMCORES= atoi(argv[1]);
    int NUMHILOS= atoi(argv[2]);
    
	// Crea una CPU 
    cpu = crearCPU(NUMCPUS, NUMCORES, NUMHILOS); 

	// Creo la cola de procesos
    colaCPU = crearCPUcola(cpu);


    colaPrioridades = crearEstructuraPrioridades(NUMPROCESOS,NUMPRIORIDADES);
    

    genProcesos_T * gp = (genProcesos_T *) malloc(sizeof(genProcesos_T));
	gp->idGeneradorProcesos = 0;

    clock_T * reloj = (clock_T *) malloc(sizeof(clock_T));
	reloj->idReloj = 0;

    temporizador_T * temporizador = (temporizador_T *) malloc(sizeof(temporizador_T));
	temporizador->idTemporizador = 0;

	scheduler_T * planificador = (scheduler_T *) malloc(sizeof(scheduler_T));
	planificador->idPlanificador = 0;

    pthread_mutex_init(&mutexProcesos, NULL);
	pthread_mutex_init(&mutexSeg, NULL);
	pthread_mutex_init(&mutex_temp, NULL);

    pthread_create(&gp->pthid,NULL,generadorProcesos,&gp->idGeneradorProcesos);
	pthread_create(&reloj->pthid,NULL,procesoReloj,&reloj->idReloj);
	pthread_create(&temporizador->pthid,NULL,procesoTemporizador,&temporizador->idTemporizador);
	pthread_create(&planificador->pthid,NULL,procesoPlanificador,&planificador->idPlanificador);

    sem_init(&seg,0,0);
	sleep(TIEMPO_MAX); 
}



