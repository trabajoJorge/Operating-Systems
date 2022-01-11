#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "PCB.h"
#include "CPU.h"
#include "ColaProcesos.h"



CPU_T * crearCPU(int id, int num_cores, int num_hilos){
        CPU_T * cpu = (CPU_T *) malloc(sizeof(CPU_T));
        cpu->id = id;
        cpu->nCores =num_cores;
        cpu->cores = (core_T *) malloc (sizeof(core_T)*num_cores);
        for (int j=0; j<num_cores; j++){
            core_T * core = (core_T *) malloc(sizeof(core_T));
            core->id=j;
            core->nHilos= num_hilos;
            core->hilos = (thread_T*) malloc (sizeof(core_T)*num_hilos);
            cpu->cores[j] = *core;
            for (int k=0; k<num_hilos; k++){
                thread_T * hilo = (thread_T *) malloc(sizeof(thread_T));
                hilo->id=k;
                hilo->estado=0;
                hilo->core_id=j;
                core->hilos[k] = *hilo;
            }
        }
	return cpu;
}

//Crea la cola de hilos libres de la cpu
colaCPU_T * crearCPUcola(CPU_T * cpu){
	int i,j;
	colaCPU_T * cola = (colaCPU_T *) malloc(sizeof (colaCPU_T));
	cola->size = 0;
	cola->threads = (thread_T **) malloc(sizeof (thread_T) * cpu->nCores*cpu->cores[0].nHilos);
	for(i=cpu->nCores-1;i>=0;i--){
		for(j=cpu->cores[i].nHilos-1;j>=0;j--){
			thread_T * hilo = (thread_T *) malloc(sizeof(thread_T));
			hilo=&cpu->cores[i].hilos[j];
			cola->threads[cola->size] = hilo;
			cola->size++;				
		}
		
	}	
	return cola;
}

//Añadimos un pcb en la cola para ocupar el hilo en esa posicion y lo sacamos de la cola
int addPCB_CPU(CPU_T * cpu, colaCPU_T * cola, PCB_T * pcb){
		if(cola->size != 0){
			cola->threads[cola->size-1]->pcb = pcb;
			cola->threads[cola->size-1]->estado =1;	
			cola->size--;			
			return 0;
		}else{
			return 1;
		}
}

//Quitamos el PCB del hilo, encolamos el hilo libre a la cola de hilos, y el pcb a la cola de procesos
int rmPCB_CPU(CPU_T * cpu, colaCPU_T * cola, colaPrioridades_T * cola_prioridades){
	for(int i=0;i<cpu->nCores;i++){
		for(int j=0;j<cpu->cores[i].nHilos;j++){		
			if(cpu->cores[i].hilos[j].estado==1){
				PCB_T * pcb = cpu->cores[i].hilos[j].pcb;
				cpu->cores[i].hilos[j].estado=0;

				if(pcb->prioridad>0){				
					pcb->prioridad--;				
				}				
				encolarProceso(cola_prioridades , pcb);

				thread_T * hilo =&cpu->cores[i].hilos[j];
				cola->threads[cola->size] = hilo;
				cola->size++;	
			}
		}
	}
	return 0;
}

//Restamos 1  al tiempo de vida de los procesos en ejecucion, si terminan se quita el proceso
//y se encola el hilo en la cola de hilos libres
int actualizarTiempo(CPU_T * cpu, colaCPU_T * cola){

	for(int i=0;i<cpu->nCores;i++){
		for(int j=0;j<cpu->cores[i].nHilos;j++){		
			if(cpu->cores[i].hilos[j].estado==1){
				if(cpu->cores[i].hilos[j].pcb->t_restante>0){
					cpu->cores[i].hilos[j].pcb->t_restante--;
				}else{
					thread_T * hilo = (thread_T *) malloc(sizeof(thread_T));
					hilo=&cpu->cores[i].hilos[j];
					cola->threads[cola->size] = hilo;
					cpu->cores[i].hilos[j].estado=0;	
					cola->size++;			
				}
					
			}			
		}
	}
	return 1;
}


//Mostramos los procesos en ejecución de la CPU, sacando cada proceso en ejecución en los hilos.
int mostrarProcesosCPU(CPU_T * cpu){
	
	int i,j;
	printf(" \n\n\n\n **** GOLPE DE RELOJ **** \n");
	printf("-------------------------------------------------------------------\n");
	for(i=0;i<cpu->nCores;i++){
		printf("-->CORE %d \n",i);
		for(j=0;j<cpu->cores[i].nHilos;j++){
			printf("---->HILO %i%d \n", i,cpu->cores[i].hilos[j].id);
			if(cpu->cores[i].hilos[j].estado==1) 
				mostrarPCB(cpu->cores[i].hilos[j].pcb); 
		}
	}
	return 0;
}

