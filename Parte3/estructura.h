#ifndef _estructura_h
#define _estructura_h

typedef struct{
	int id_load;
	pthread_t pthid;
}loader_T;

typedef struct {
	int id_clock;
	pthread_t pthid;
}clock_T;

typedef struct{
	int id_temp;
	pthread_t pthid;
} temporizador_T;

typedef  struct
{
	int id_sch;
	pthread_t pthid;
}scheduler_T;

void * generador_Procesos(void *id_gen);

#endif