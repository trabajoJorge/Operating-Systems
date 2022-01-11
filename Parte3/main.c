#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    
#include <time.h>   
#include <string.h>
#include "CPU.h"
#include "PCB.h"
#include "PRIORIDADES.h"
#include "estructura.h"
#include "memoria.h"
#include "defines.c"



sem_t seg;
pthread_mutex_t mutex_seg, mutex_temp, mutex_procesos;
struct COLA_PRIORIDADES * cola_prioridades;
CPU_T * cpu;
struct CPUQUEUE * colaCPU;
 memoria_T * memoria;
int temporizador;
int interr_temp;
int interr_process;
FILE * f;
int total_pag=0;

// Hilo Loader, su objetivo es la carga en memoria de los procesos (archivos .elf), decodificarlos, crear la estructura
// necesaria para la creacion del proceso y encolarlos en la cola de prioridades.

void * lloader(void *id_load) {

	char nombre_prog[] = PRIMER_PROG;	
	
	int pcb_pid=0;
    while((f=fopen(nombre_prog,"r"))!=NULL) {
		if (TRAZA_LECTURA){
			("\n\n-------------------------------------------------------------------\n");
			printf("LECTURA DE PROGRAMA %s \n",nombre_prog);
		}

		// Se inicializan los valores para la lectura de los ficheros
		int i=0;
		int numli_text=0;
		int numli_datos=0;
		int PC =0; 	// Al contar con una unica CPU no se ha valorado añadirlo a cada CPU por lo que unicamente se inicializa a 0 y se usa
					// para la unica CPU del sistema.

		char valor_linea_char[TAM_LINEA];
		char valor_contenido_cabecera[TAM_DATO_CABECERA];
		int valor_linea_int;

		// Se crean dos array con el maximo de elementos que pueda tener un fichero para no quedarse corto de espacio
		// quedandose de esta manera con posiciones vacias
		int int_textos[MAX_LINEAS];
		int int_datos[MAX_LINEAS];

		// Se calcula la posicion donde comienzan las insturcciones
		fgets(valor_linea_char,TAM_LINEA,f);
		strncpy(valor_contenido_cabecera,valor_linea_char+6,6);
		valor_linea_int = (int)strtol(valor_contenido_cabecera,NULL,16);
		if (TRAZA_LECTURA) printf("|-- INICIO INSTRUCCIONES [0x%06X] \n",valor_linea_int);

		// Se calcula la posicion donde comienzan los datos
		fgets(valor_linea_char,TAM_LINEA,f);		
		strncpy(valor_contenido_cabecera,valor_linea_char+6,6);
		valor_linea_int = (int)strtol(valor_contenido_cabecera,NULL,16);
		if (TRAZA_LECTURA) printf("|-- INICIO DATOS [0x%06X] \n",valor_linea_int);

		// Se recogen los datos de tipo text
		while(valor_linea_int>PC){
			fgets(valor_linea_char,TAM_LINEA,f);			
			int_textos[i]=(int)strtol(valor_linea_char,NULL,16);
			if (TRAZA_LECTURA) printf("\t|--> GUARDADA LINEA DE TEXTO : %08X \n",int_textos[i]);
			PC+=TAM_PALABRA;
			i++;
			numli_text++;
		}
		i=0;
		// Se recogen los datos de tipo data
		while(fgets(valor_linea_char,TAM_LINEA,f)!=NULL){
			int_datos[i]=(int) strtol(valor_linea_char,NULL,16);
			if (TRAZA_LECTURA) printf("\t|--> GUARDADA LINEA DE DATOS : %08X \n",int_datos[i]);
			i++;
			numli_datos++;
		}

		// Se muestran los resultados de la lectura
		int tam_total = numli_datos+numli_text;
		if (TRAZA_LECTURA) printf("|--> LINEAS TOTALES : %d \n",tam_total);
		if (TRAZA_LECTURA) printf("|--> LINEAS TEXTO : %d \n", numli_datos);
		if (TRAZA_LECTURA) printf("|--> LINEAS DATOS : %d \n", numli_text);
		
		// Se cierra el fichero
		fclose(f);

		// Se calcula el numero de paginas total
		int num_pag;
        if (tam_total%64 != 0) num_pag = tam_total/64 + 1;
        else num_pag = tam_total/64;
		total_pag+=num_pag;

		if (TRAZA_LECTURA) printf("|--> NUMERO DE PAGINA A ESCRIBIR : %d\n",total_pag);
		
		// Se escriben en memoria los datos recogidos
		int PTBR = escribirMemoria(memoria,num_pag, int_textos, numli_text,int_datos, numli_datos);
		// Se crea la estructura MM
		struct MM * mm = crearMM(numli_text,numli_datos,PTBR);
		
		// Se crea un PCB, con los datos anteriores y prioridades y quantum, aleatorios. 
		int prio = (rand()%NUMPRIORIDADES);      
        int quantum = (rand()%QUANTUMMAX);    
		struct PCB * pcb = crearPCB(pcb_pid,numli_text,prio,quantum,mm);

		// Se calcula el nombre del siguiente programa
		int j;
		char * num_prog = (char *)malloc(sizeof(char));
		for(j=0;j<3;j++)num_prog[j]=nombre_prog[4+j];
		char np[4];
		strncpy(np,&nombre_prog[4],3);
		np[3] = '\0';
		if (TRAZA_LECTURA) printf("|--> PROGRAMA ACTUAL : %s\n",nombre_prog);
		int val =  (int)strtol(np ,NULL,10);
		val++;
		sprintf(num_prog, "%d", val);
		
		// Dependiendo si el numero del programa es menor o mayor que 10 le concateno un 0 o dos
		strcpy(nombre_prog, "prog");		
		if(strlen(num_prog)<2){
			strcat(nombre_prog, "00");
		}else{
			strcat(nombre_prog, "0");
		}
		strcat(nombre_prog, num_prog);		
		strcat(nombre_prog, ".elf");
		if (TRAZA_LECTURA) printf("|--> SIGUIENTE PROGRAMA : %s \n", nombre_prog);

		// Se encola el pcb en la cola de procesos para su posterior ejecucion, y al tener que hacerse en
		// una seccion critica se bloquea mediante semaforos para evitar problemas
		pthread_mutex_lock(&mutex_procesos);
			encolar_proceso(cola_prioridades, pcb);		
			interr_process=1;
        pthread_mutex_unlock(&mutex_procesos);
		int espera = ((rand()%TIEMPOESPERA) +1);
		pcb_pid++;
		sleep(espera);		

    }	
	if (TRAZA_LECTURA) printf("\n **** FINALIZA LECTURA DE PROGRAMAS ****\n");
	if (TRAZA_LECTURA) ("\n\n-------------------------------------------------------------------\n");
	
}

void *scheduler(void *s) {
    int i,j;
    while(1){
		pthread_mutex_lock(&mutex_procesos);				
		if(interr_process==1){			
			for(i=0;i<cpu->num_cores;i++){
				for(j=0;j<cpu->cores[i].num_hilos;j++){				
					struct PCB *pcb =desencolar_primer_proceso(cola_prioridades);
					if(pcb!=NULL){
						agregarProceso_PCB_CPU(cpu, colaCPU, pcb);						
					}
				}
			}
			interr_process=0;
		}
        pthread_mutex_unlock(&mutex_procesos);
    }
}


void *cclock(void *id_clock) {   
    while(1) {
		if(TRAZA_INSTRUCCIONES || TRAZA_LECTURA || TRAZA_MEMORIA || TRAZA_CPU) ("\n\nESTADO DEL SISTEMA \n");
		sleep(1);
		system("clear");
        pthread_mutex_lock(&mutex_seg);       
			ejecutar_instruccion(cpu,colaCPU,memoria);
			pthread_mutex_lock(&mutex_procesos);
				interr_process = 1;
			pthread_mutex_unlock(&mutex_procesos);
        pthread_mutex_unlock(&mutex_seg);
		sem_post(&seg);
 
		// Se muestran los procesos en ejecucion en cada hilo dentro de su correspondiente core.
		if (TRAZA_CPU) mostrar_procesosCPU(cpu);
		// Se muestra el contenido de la memoria.
		if(TRAZA_MEMORIA) mostrar_memoria(memoria,total_pag);
    }
}

void *ttemp(void *id_temp) {
    while(1) {
		sem_wait(&seg);
		pthread_mutex_lock(&mutex_seg);
		temporizador++;
		if(temporizador==TIEMPO_ACTUALIZAR){
			temporizador=0;			 
			interr_temp=1;		
			printf("\n");
			printf(" ACTUALIZADO \n");	
			printf("\n");
		}
		pthread_mutex_unlock(&mutex_seg);
    }
}






int main(int argc, char *argv[]){
	if (argc!=3) {
        printf("ERROR: El numero de argumentos es incorrecto");
        exit(-1);
    }

    int NUMCPUS= 1;
    int NUMCORES= atoi(argv[1]);
    int NUMHILOS= atoi(argv[2]);
	
	cpu = crearCPU(1,NUMCORES,NUMHILOS);
	colaCPU =  create_CPUQUEUE(cpu);
	cola_prioridades = crear_cola_prioridades(NUMPROCESOS, NUMPRIORIDADES);
	memoria = crearMemoria(24);

	// Se crea el gestor de procesos nuevo (loader)
	loader_T * gp = (loader_T *) malloc(sizeof(loader_T));
	gp->id_load = 0;
	clock_T * clk = (clock_T *) malloc(sizeof(clock_T));
	clk->id_clock = 0;
	 temporizador_T * temp = ( temporizador_T *)malloc(sizeof( temporizador_T));
	temp->id_temp = 0;
	 scheduler_T * sch = ( scheduler_T *)malloc(sizeof( scheduler_T));
	sch->id_sch = 0;
	
	pthread_mutex_init(&mutex_procesos, NULL);
	pthread_mutex_init(&mutex_seg, NULL);
	pthread_mutex_init(&mutex_temp, NULL);
	
	// Se crea el thread que lanza el proceso loader
	pthread_create(&gp->pthid,NULL,lloader,&gp->id_load);
	
	pthread_create(&clk->pthid,NULL,cclock,&clk->id_clock);
	pthread_create(&temp->pthid,NULL,ttemp,&temp->id_temp);
	pthread_create(&sch->pthid,NULL,scheduler,&sch->id_sch);

	sem_init(&seg,0,0);
	sleep(TIME_TO_EXIT);

	printf("\n **** TIEMPO MAXIMO EXCEDIDO ****\n");
}