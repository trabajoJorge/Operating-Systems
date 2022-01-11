#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    
#include <time.h>  
#include "memoria.h"

#define INI_KERNEL 0x000000
#define FIN_KERNEL 0x3FFFFF
#define INI_PAG 0x000000


// Funcion encargada de la creacion y asignacion de valores iniciales a la memoria.
 memoria_T * crearMemoria(int size_bits) {
     memoria_T * memoria = ( memoria_T *)malloc(sizeof( memoria_T));
    // TODO: ¿La memoria empieza en la posicion 0 o la del enunciado?
    memoria->ini_kernel = INI_KERNEL;
    memoria->fin_kernel = FIN_KERNEL;  
    memoria->ini_pag = INI_PAG;
    int tam = 1 << size_bits;
    memoria->contenido = (int *)malloc(sizeof(int)*tam);
    memoria->tam = tam;  
    memoria->fin_pag = tam/256-1;
    return memoria;
}


// Funcion encargada de comprobar si hay espacio dada una memoria y un numero de pagina.
int comprobar_espacio( memoria_T * memoria,int num_pag){
    int desplazamiento=0;
    int hay_sitio=0;
    int pag_actual=0;
    int cont_pagina =num_pag;  
    while(hay_sitio ==0 && desplazamiento<memoria->fin_pag){       
        if(memoria->contenido[INI_PAG+desplazamiento]==1){
            desplazamiento++;
            pag_actual=desplazamiento;
        }else{
            cont_pagina--;
            if(cont_pagina==0) hay_sitio=1;
            else desplazamiento++;
        }
    }    
    if(hay_sitio==0)return -1;    
    return pag_actual;
}


// Funcion encargada de escribir en memoria los datos de los procesos recogidos anteriormente.
int escribirMemoria( memoria_T * memoria,int num_pag, int int_textos[MAX_LINEAS], int numli_text, int int_datos[MAX_LINEAS], int numli_data){
    // Se comprueba si hay espacio
    int pag_actual = comprobar_espacio(memoria,num_pag);  
    
    // Si hay espacio
    if(pag_actual!=-1){
        int h;
        // Se ocupa la memmoria.
        for(h=0;h<num_pag;h++) memoria->contenido[pag_actual+h] = 1;

        // Se agregan las instrucciones.
        for(h=0;h<numli_text;h++) memoria->contenido[FIN_KERNEL+1+TAM_PAGINA*pag_actual+h*TAM_PALABRA]=int_textos[h];
        
        // Se agregan los datos.
        for(h=0;h<numli_data;h++){
            memoria->contenido[FIN_KERNEL+1+TAM_PAGINA*pag_actual+h*TAM_PALABRA+numli_text*TAM_PALABRA] = int_datos[h];
        }
    // Si no hay espacio.
    }else{
        printf( "**** FALTA ESPACIO PARA ALMACENAR EL PROGRAMA **** \n");
    }
    return pag_actual;
}

// Funcion encargada de mostrar por salida estandar el contenido de la memoria.
void mostrar_memoria( memoria_T * memoria,int num_pag){
    printf("\n\n CONTENIDO DE MEMORIA \n");
    int dir_virt;
    int cont = 0;
    // Se recorren las paginas de la memoria, es decir, las posiciones de la misma de 4 en 4.
    for(dir_virt=INI_PAG;dir_virt<num_pag*TAM_PAGINA;dir_virt+=4){
        int direccion_mem = memoria->fin_kernel+1+dir_virt;
        // Se muestra la posicion de la memoria con su contenido.
        printf("\t|--> [0x%06X] : %08X \n",direccion_mem,memoria->contenido[direccion_mem]);
    }
}