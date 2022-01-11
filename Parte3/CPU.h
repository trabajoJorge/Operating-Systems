#include "PRIORIDADES.h"
#include "memoria.h"

#ifndef _CPU_h
#define _CPU_h

typedef  struct {
    int id;
	int core_id;
    int estado; //1ocupado 0 libre
    struct PCB *pcb;   
    // Se crean las nuevas estructuras
    int PC; 
    int PTBR;
    int * registros;
} thread_T;

typedef struct {
    int id;
    int num_hilos;
    thread_T *hilos;
} core_T;

typedef struct {
    int id;
    int num_cores;
    core_T *cores;
} CPU_T;

struct CPUQUEUE {
	int size;
	thread_T **hilos;
};

CPU_T * crearCPU(int id, int num_cores, int num_hilos);
int mostrar_procesosCPU(CPU_T * cpu);
struct CPUQUEUE * create_CPUQUEUE(CPU_T * cpu);

int quitarProcesos_PCB_CPU(CPU_T * cpu, struct CPUQUEUE * cola, struct COLA_PRIORIDADES * cola_prioridades );
int agregarProceso_PCB_CPU(CPU_T * cpu, struct CPUQUEUE * cola, struct PCB * pcb);
int desencolar_PCB_CPU(CPU_T * cpu, struct CPUQUEUE * cola);

int ejecutar_instruccion(CPU_T * cpu, struct CPUQUEUE * cola,  memoria_T * memoria);
void leer_instruccion( memoria_T * memoria, struct PCB * pcb,thread_T * hilo);



#endif