#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "PCB.h"
#include "ColaProcesos.h"


colaPrioridades_T * crearEstructuraPrioridades(int num_procesos, int prioridad){
	
	colaPrioridades_T * colaPrioridades = (colaPrioridades_T *) malloc(sizeof(colaPrioridades_T));
	colaPrioridades->procesos = (colaProcesos_T *) malloc(sizeof(colaProcesos_T)*prioridad);
	colaPrioridades->size = prioridad;
	

	for(int i = 0;i<prioridad;i++){
		colaProcesos_T * colaProcesos = (colaProcesos_T *) malloc(sizeof(colaProcesos_T));
		colaProcesos->pcbs = (PCB_T *) malloc(sizeof(PCB_T)*num_procesos);
		colaProcesos->num_procesos = num_procesos;
		colaProcesos->size = 0;
		colaProcesos->prioridad = prioridad;
		colaProcesos->pos_actual = 0;			
		colaProcesos->ultimo = 0;
		colaPrioridades->procesos[i]= *colaProcesos;
	}
	return colaPrioridades;
} 

int encolarProceso(colaPrioridades_T * colaPrioridades , PCB_T * pcb){
	colaPrioridades->procesos[pcb->prioridad].pcbs[colaPrioridades->procesos[pcb->prioridad].ultimo] = *pcb;	
	colaPrioridades->procesos[pcb->prioridad].size++;
	colaPrioridades->procesos[pcb->prioridad].ultimo++;
	return 0;
}

PCB_T * desencolarProceso(colaPrioridades_T * cola){ //primero
	int i;	
	for(i=0;i<cola->size;i++){
		if(cola->procesos[i].size >0){			
			PCB_T * pcb = &cola->procesos[i].pcbs[cola->procesos[i].pos_actual];
			cola->procesos[i].pos_actual++;
			cola->procesos[i].size--;
			return pcb;
		}
	}
	return NULL;
}