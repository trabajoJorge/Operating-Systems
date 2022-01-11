#include "ColaProcesos.h"

#ifndef _CPU_h
#define _CPU_h




typedef struct{
    int id;
	int core_id;
    int estado; //1 ocupado 0 libre
    PCB_T *pcb;    
}thread_T;

typedef struct{
    int id;
    int nHilos;
    thread_T *hilos;
}core_T;

typedef struct{
    int id;
    int nCores;
    core_T *cores;
}CPU_T;


typedef struct{
    thread_T **threads;
    int size;
}colaCPU_T;

CPU_T * crearCPU(int, int, int);
colaCPU_T * crearCPUcola(CPU_T * cpu);

typedef struct{
	int idGeneradorProcesos;
	pthread_t pthid;
} genProcesos_T;

typedef struct {
	int idReloj;
	pthread_t pthid;
} clock_T;

typedef struct{
	int idTemporizador;
	pthread_t pthid;
	
} temporizador_T;

typedef struct scheduler{
	int idPlanificador;
	pthread_t pthid;
} scheduler_T;

void * generador_Procesos(void *id_gen);

colaCPU_T * crearCPUcola(CPU_T * cpu);
int addPCB_CPU(CPU_T * cpu, colaCPU_T * cola, PCB_T * pcb);
int rmPCB_CPU(CPU_T * cpu, colaCPU_T * cola, colaPrioridades_T * cola_prioridades);
int mostrarProcesosCPU(CPU_T * cpu);
int actualizarTiempo(CPU_T * cpu, colaCPU_T * cola);

#endif
