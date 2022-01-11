#ifndef _PCB_h
#define _PCB_h

struct PCB {
	int id;
	int t_vida;
	int t_restante;
	int quantum;
	int estado; //1 ocupado, 0 ready
	int prioridad;	

	// Se instancian las estructuas adicionales.
	struct MM * mm;	
	int PC; // Procces Controller.
    int PTBR; // Identificador a la pagina libre.
};

struct MM{
	int codigo; // Direccion virtual de comienzo de segmento de codigo.
	int datos; 	// Direccion virtual de comienzo de segmento de datos.
	int pgb;	// Direccion fisica correspondiente a la tabla de paginas.
};


struct PCB * crearPCB(int pid, int t_vida, int prioridad, int quantum,struct MM * mm);
struct MM * crearMM(int codigo, int datos, int pgb);
int mostrar_PCB(struct PCB * pcb);

#endif