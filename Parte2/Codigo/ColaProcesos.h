#ifndef _ColaProcesos_h
#define _ColaProcesos_h


typedef struct{ 
	int num_procesos;
	int size;
	PCB_T* pcbs;
	int prioridad;	
	int pos_actual;
	int ultimo;
} colaProcesos_T;

typedef struct{
	colaProcesos_T * procesos;
	int size;
} colaPrioridades_T;

colaPrioridades_T * crearEstructuraPrioridades(int num_procesos, int prioridad);
int encolarProceso(colaPrioridades_T * cola , PCB_T * pcb);
PCB_T * desencolarProceso(colaPrioridades_T * cola);

#endif