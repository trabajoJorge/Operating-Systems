#ifndef _PCB_h
#define _PCB_h

typedef struct {
	int id;
	int t_vida;
	int t_restante;
	int quantum; 
	int state; //0 listo y 1 ocupado
	int prioridad;	
} PCB_T;

PCB_T * crearPCB(int pid, int t_vida, int prioridad, int quantum);

void* mostrarPCB(PCB_T * pcb);
#endif
