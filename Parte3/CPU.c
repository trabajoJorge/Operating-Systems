#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "CPU.h"
#include "PCB.h"
#include "PRIORIDADES.h"
#include "defines.c"

#define try bool __HadError=false;
#define catch(x) ExitJmp:if(__HadError)
#define throw(x) __HadError=true;goto ExitJmp;

// Funcion encargada de la creacion de una CPU con sus respectivos hilos y cores
CPU_T * crearCPU(int id, int num_cores, int num_hilos){
	int i,j;
	CPU_T * cpu = (CPU_T *) malloc(sizeof(CPU_T));
	cpu->id = id;
	cpu->num_cores =num_cores;
	cpu->cores = (core_T *) malloc (sizeof(core_T)*num_cores);
	for(i=0;i<num_cores;i++){
		core_T * core = (core_T *)malloc(sizeof(core_T));
		core->id = i;
		core->num_hilos = num_hilos;
		core->hilos = (thread_T *) malloc (sizeof(thread_T)*num_hilos);
			for(j=0;j<num_hilos;j++){
				thread_T * hilo = (thread_T *) malloc(sizeof(thread_T));
				hilo->registros = (int *) malloc(sizeof(int)*16);
				hilo->id = j;
				hilo->core_id= i;
				hilo->estado = 0;
				core->hilos[j] = *hilo;
			}
		cpu->cores[i] = *core;
	}
	return cpu;	
}

//  Funcion encargada de mostrar los procesos en ejecucion de cada hilo en su respectivo core
int mostrar_procesosCPU(CPU_T * cpu){
	int i,j;
	for(i=0;i<cpu->num_cores;i++){
		printf("\t|--> CORE %d: \n",i);
		for(j=0;j<cpu->cores[i].num_hilos;j++){
			printf("\t\t|--> HILO %d%d \n", i, cpu->cores[i].hilos[j].id);
			if(cpu->cores[i].hilos[j].estado==1){
				mostrar_PCB(cpu->cores[i].hilos[j].pcb);
			}	
		}
	}
	return 0;
}

// Funcion encargada de crear una cola ordenada con todos los hilos libre s
struct CPUQUEUE * create_CPUQUEUE(CPU_T * cpu){
	int i,j;
	struct CPUQUEUE * cola = (struct CPUQUEUE *) malloc(sizeof(struct CPUQUEUE));
	cola->size = 0;
	cola->hilos = (thread_T **) malloc(sizeof(thread_T)*cpu->num_cores*cpu->cores[0].num_hilos);
	for(i=cpu->num_cores-1;i>=0;i--){
		for(j=cpu->cores[i].num_hilos-1;j>=0;j--){
			thread_T * hilo = (thread_T *) malloc(sizeof(thread_T));
			hilo=&cpu->cores[i].hilos[j];
			cola->hilos[cola->size] = hilo;
			cola->size++;			
		}
	}	
	return cola;
}

// Funcion encargada de meter un PCB en el ultimo hilo de la cola ocupandolo de esta manera y quitandolo de la cola
// de procesos.
int agregarProceso_PCB_CPU(CPU_T * cpu, struct CPUQUEUE * cola, struct PCB * pcb){
		if(cola->size != 0){
			// TODO: Falla aquí, cuando se llenan todos los hilos
			cola->hilos[cola->size-1]->pcb = pcb;

			cola->hilos[cola->size-1]->estado =1;	
			cola->size--;			
			return 0;
		}else{
			return 1;
		}
}

// Funcion encargada de quitar el PCB del hilo, volviendo a meter el PCB en la cola de prioridades y metiendo este hilo ahora
// libre en nuestra cola de prioridades
int quitarProcesos_PCB_CPU(CPU_T * cpu, struct CPUQUEUE * cola, struct COLA_PRIORIDADES * cola_prioridades ){
	int i,j;
	for(i=0;i<cpu->num_cores;i++){
		for(j=0;j<cpu->cores[i].num_hilos;j++){		
			if(cpu->cores[i].hilos[j].estado==1){
				struct PCB * pcb = cpu->cores[i].hilos[j].pcb;
				cpu->cores[i].hilos[j].estado=0;						
				if(pcb->prioridad>0) pcb->prioridad--;				
				encolar_proceso(cola_prioridades , pcb);
				thread_T * hilo =&cpu->cores[i].hilos[j];
				cola->hilos[cola->size] = hilo;
				cola->size++;	
			}
		}
	}
	return 0;
	
}

// Se ejecutan las instrucciones escribiendo o consultando lo necesario en memoria
// Para ello se entiende que cada instruccion tarda un golpe de reloj en ejecutarse
int ejecutar_instruccion(CPU_T * cpu, struct CPUQUEUE * cola,  memoria_T * memoria){
	int i,j;
	if (TRAZA_INSTRUCCIONES) printf("INSTRUCCIONES EN EJECUCION\n");
	for(i=0;i<cpu->num_cores;i++){
		for(j=0;j<cpu->cores[i].num_hilos;j++){		
			if(cpu->cores[i].hilos[j].estado==1){
				if(cpu->cores[i].hilos[j].pcb->t_restante>0){
					leer_instruccion(memoria,cpu->cores[i].hilos[j].pcb,&cpu->cores[i].hilos[j]);
					cpu->cores[i].hilos[j].pcb->t_restante--;
				}else{
					thread_T * hilo = (thread_T *) malloc(sizeof(thread_T));
					hilo=&cpu->cores[i].hilos[j];
					// TODO: Falla aquí
					cola->hilos[cola->size] = hilo;
					cpu->cores[i].hilos[j].estado=0;	
					cola->size++;			
				}
			}			
		}
	}
	return 1;
}

// Funcion encargada de leer y ejecutar las instrucciones leidas desde nuestra memoria
void leer_instruccion( memoria_T * memoria, struct PCB * pcb,thread_T * hilo){
	int i;
	char * contenido = (char *)malloc(sizeof(char));
	sprintf(contenido, "%X", memoria->contenido[1+memoria->fin_kernel+pcb->PC+pcb->PTBR*TAM_PAGINA]);
	char * resultado=(char *)malloc(sizeof(char));
	for(i=0; i<8-strlen(contenido);i++){
		strcat(resultado, "0");	
	}
	strcat(resultado,contenido);

	int r1=0;
	int r2=0;
	int r3=0;
	char direc_dato[6];
	int direc_mem;
	int aux;
	int posicion_f; 
	int posicion_a;	
	int suma=0;
	// TODO: Creo que fallan los load y los store al buscar en la memoria y traer el dato
	switch(resultado[0]){
		case '0':		
			r1 = resultado[1]-'0';		
			strncpy(direc_dato,&resultado[2],6);	
		
			aux =  (int)strtol(direc_dato,NULL,16);
			//printf("AUX  = %d \n",aux);
			hilo->registros[r1]=memoria->contenido[1+memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux];		
			//posicion_f = 1+memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux;
			//printf("DIRECCION REAL %08X \n",posicion_f);
			//printf("EL VALOR QUE HA TOMADO R1 DE LA DIRECCION REAL ES %d \n", hilo->registros[r1]);
			if (TRAZA_INSTRUCCIONES) printf("|--> [ ld , r%d , %0X0]   \n",r1,1+memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux);
			break;

		case '1':
			//	printf("Esto es un Storage \n");
			r1 = resultado[1]-'0';		
			//printf("R1 -> %d \n", r1);		
			strncpy(direc_dato,&resultado[2],6);				
			aux =  (int)strtol(direc_dato,NULL,16);
			//printf("AUX  = %d \n",aux);		
			memoria->contenido[memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux+pcb->mm->codigo]=hilo->registros[r1];
			//	printf("EL VALOR QUE HA TOMADO LA DIRECCION %08X  ES %d \n",memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux+pcb->mm->codigo, hilo->registros[r1]);
			if (TRAZA_INSTRUCCIONES) printf("|--> [ st , r%d , %0X0]   \n",r1,memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux+pcb->mm->codigo);
			break;

		case '2':
			//printf("Esto es un add \n");
			r1 = resultado[1]-'0';	
			r2 = resultado[2]-'0';
			r3 = resultado[3]-'0';	
			//printf("R1 -> %d \n", r1);	

			suma = hilo->registros[r2]+hilo->registros[r3];
			//printf("SE VA A SUMAR %d + %d \n",hilo->registros[r2],hilo->registros[r3]);
			hilo->registros[r1] = suma; 
			//printf("RESULTADO : %d", hilo->registros[r1]);
			if (TRAZA_INSTRUCCIONES) printf("|--> [ add , r%d , r%d, r%d ]   \n",r1,r2,r3);
			break;
		case 'F':
			if (TRAZA_INSTRUCCIONES) printf("|--> FIN DEL PROCESO (Instruccion exit)\n");

			break;
		
	}
	pcb->PC+=4;
}








