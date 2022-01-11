#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "PCB.h"

// Funcion encargada de crear una PCB incluyendo las nuevas estructuras
struct PCB * crearPCB(int pid, int t_vida, int prioridad, int quantum,struct MM * mm){
	struct PCB * pcb = (struct PCB *) malloc (sizeof(struct PCB));
	pcb->id = pid;
	pcb->t_vida = t_vida;
	pcb->t_restante = pcb->t_vida;

	// Se crean las estructuras adicionales
	pcb->PTBR=mm->pgb; 
	pcb->mm=mm;
	pcb->quantum = quantum;
	pcb->estado = 0; 
	pcb->prioridad = prioridad;	
	pcb->PC=0;
	
	return pcb;
}


// Funcion encargada de crear una estructura MM
struct MM * crearMM(int codigo, int datos, int pgb){
	struct MM * mm = (struct MM *) malloc(sizeof(struct MM));
	mm->codigo=codigo;
	mm->datos = datos;
	mm->pgb = pgb;	
	return mm;
}

// Se muestra por salida estandar los datos de la PBC
int mostrar_PCB(struct PCB * pcb){
	printf("\t\t\t|--> PCB -> id : %d, Tiempo de vida : %d, Tiempo restante : %d, Quantum : %d, Estado : %d, Prioridad : %d \n", pcb->id,pcb->t_vida,pcb->t_restante,pcb->quantum,pcb->estado,pcb->prioridad);
	printf("\t\t\t      |--> MM -> Codigo: %d, Datos: %d, PGB: %d \n",pcb->mm->codigo,pcb->mm->datos,pcb->mm->pgb);
	return 1;
}