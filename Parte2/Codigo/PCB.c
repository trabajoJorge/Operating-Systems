#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "PCB.h"
#include "defines.c"

PCB_T * crearPCB(int pid, int t_vida, int prioridad, int quantum){
	PCB_T * pcb = (PCB_T *) malloc (sizeof(pcb));
	pcb->id = pid;
	pcb->t_vida = t_vida;
	pcb->t_restante = pcb->t_vida;
	pcb->quantum = quantum;
	pcb->state = 0;
	pcb->prioridad = prioridad;
		
	return pcb;
}

void* mostrarPCB(PCB_T * pcb){
	printf("        |--> PCB%d --> Tiempo de vida = %d, Tiempo restante = %d, Quantum = %d Prioridad = %d \n",
	pcb->id,pcb->t_vida,pcb->t_restante, pcb->quantum,pcb->prioridad);
	if (pcb->id == NUMPROCESOS-1){
		printf("\n +-------------------------------------+\n");
		printf(  " |    TODOS LOS PROCESOS EJECUTADOS    |\n");
		printf(" +-------------------------------------+\n\n");
		exit(1);
	}
}