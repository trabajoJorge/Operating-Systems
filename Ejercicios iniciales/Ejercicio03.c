
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

void matrix_init(int **matr, int rm, int cm);
void matrix_processing(int **matr, int rm, int cm);
void display_matrix(int **matr, int rm, int cm);
int ___dont_get_me_wrong(char *s);
void gimme_all_your_lovin(int argc, char *argv[]);

#define ROW_DEFAULT 2
#define COLUMN_DEFAULT 4

int **matrix;
int column_max;
int row_max;



int main (int argc, char *argv[]){
    int f;
    /* Toma y validación de prámetros */
    gimme_all_your_lovin(argc, argv);
    printf(" row_max: 0x%04x(%4d)\ncolumn_max: 0x%04x(%4d)\n\n", row_max, row_max, column_max, column_max);

    /* Creación de la matriz dinámica */
    matrix = (int **)malloc (row_max*sizeof(int *));
    for (f=0; f<row_max; f++) matrix[f] = (int *) malloc (column_max*sizeof(int));

    /* tratamientos */
    matrix_init(matrix, row_max, column_max);
    display_matrix(matrix, row_max, column_max);
    matrix_processing(matrix, row_max, column_max);
    display_matrix(matrix, row_max, column_max);

    /* Liberar espacio de la matriz dinámica */
    for (f=0; f<row_max; f++) free(matrix[f]);
    free(matrix);
}


void matrix_init(int **matr, int rm, int cm){
    int f,c;
    for (f=0; f<rm; f++) for (c=0; c<cm; c++) matr[f][c] = 0x61 + (f * cm) + c;
}

void matrix_processing(int **matr, int rm, int cm){
    int f,c;
    for (f=0; f<rm; f++)for (c=0; c<cm; c++) matr[f][c] = matr[f][c] & 0xDF;
}

void display_matrix(int **matr, int rm, int cm){
    int f,c;
    for (f=0; f<rm; f++) {
        for (c=0; c<cm; c++){
            printf("[%d,%d] 0x%04x %c -- ", f, c, matr[f][c], (char)matr[f][c]);
        }
        printf("\n");
    }
    printf("\n");
} 

int ___dont_get_me_wrong(char *s) {
 printf("***error*** %s\n",s);
 exit(-1);
} 

void gimme_all_your_lovin(int argc, char *argv[]){ // tomar parámentros
    int opt, long_index;
    static struct option long_options[] = {
        {"column", required_argument, 0, 'c' },
        {"row", required_argument, 0, 'r' },
        {"help", no_argument, 0, 'h' },
        {0, 0, 0, 0 }
    };

    column_max = COLUMN_DEFAULT;
    row_max = ROW_DEFAULT;

    long_index =0;
    while ((opt = getopt_long(argc, argv,":hc:r:", long_options, &long_index )) != -1) {
        /*-----------------------------------
        * put ':' in the starting of the
        * string so that program can
        * distinguish between '?' and ':'
        *----------------------------------*/
        switch(opt) {
            case 'c': /* -c or --column */
                column_max = atoi(optarg);
                if ((column_max < 0) || (column_max > 4)) {
                    ___dont_get_me_wrong("Columnas: Fuera de rango");
                }
                column_max = 1 << column_max;   
                break;
            case 'r': /* -r or --row */
                row_max = atoi(optarg);
                if ((row_max < 0) || (row_max > 4)) {
                    ___dont_get_me_wrong("Filas: Fuera de rango");
                }
                row_max = 1 << row_max;
                break;
            case 'h': /* -h or --help */
            case '?':
                printf ("Uso %s [OPTIONS]\n", argv[0]);
                printf (" -c --column=NNN\t"
                "Nº de columnas [%d]\n", COLUMN_DEFAULT);
                printf (" -h, --help\t\t"
                "Ayuda\n");
                printf (" -r --row=NNN\t\t"
                "Nº de filas [%d]\n", ROW_DEFAULT);
                exit (2);
            default:
                ___dont_get_me_wrong("Unknown argument option");
        }
    }
} 