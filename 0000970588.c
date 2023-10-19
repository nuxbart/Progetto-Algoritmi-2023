/*
    Cognome: BARTOLUCCI
    Nome: ANNA
    Matricola: 0000970588
    Gruppo: A
    Emial: anna.bartolucci@studio.unibo.it
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_N 500
#define MAX_M 500
#define MIN_N 10
#define MIN_M 10

/* Struct per rappresentare una cella della matrice*/
struct Cell{
    int x;
    int y;
};

/* Variabili globali*/
int N, M;                       /*Dimensioni della matrice in input*/
int dir_x[] = {1, -1, 0, 0};    /*Vettore delle direzioni per la x*/
int dir_y[] = {0, 0, 1, -1};    /*vettore delle direzioni per la y*/
char directions[] = "NSOE";     /*Stringa delle direzioni per output*/
int head = 0, tail = 0;         /*Indici della coda*/

int **matrix;                   /*Matrice di input*/
int **rain;                     /*Matrice della pioggia*/
bool **visited;                 /*Matrice delle celle visitate dal BFS*/
int **dist;                     /*Matrice delle distanze dalla cella di partenza ad ogni altra*/
struct Cell* bfsQueue;          /*Coda per la BFS*/
char *path;                     /*Array per memorizzare il camminio*/


/*Funzione per leggere gli input da file. Si leggono i valori dal file di input e vengono
  inizializzate le matrici: matrix, con i valori presi da input, rain, settando inizialmente
  tutti i valori a 0, e visited, settando tutte le celle a false.*/
void read_from_file(char* filename) {
    int i, j;
    FILE* fp = stdin;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Errore!\nFile di input non aperto correttamente!\n");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d %d", &N, &M);

    /*Controllo i valori di input*/
    if (N < MIN_N || M < MIN_M || M > MAX_M || N > MAX_N){
        fprintf(stderr, "Errore!\nDimensioni della matrice di input non valide!\n");
        exit(EXIT_FAILURE);
    }

    /*Allocazione dinamica della memoria per le matrici*/
    matrix = malloc(N * sizeof(int*));
    rain = malloc(N * sizeof(int*));
    visited = malloc(N * sizeof(bool*));
    dist = malloc(N * sizeof(int*));

    for (i = 0; i < N; i++) {
        matrix[i] = malloc(M * sizeof(int));
        rain[i] = malloc(M * sizeof(int));
        visited[i] = malloc(M * sizeof(bool));
        dist[i] = malloc(M * sizeof(int));
        }

    /*Inizializzazione delle matrici*/
    for (i=0; i < N; i++) {
        for (j = 0; j < M; j++) {
            int num = 0;
            fscanf(fp, "%1d", &num);
            matrix[i][j] = num;
            rain[i][j] = 0;
            visited[i][j] = false;
            dist[i][j] = 0;
        }
    }

    fclose(fp);
}

/* Funzione che stampa il cammino minimo che va da start a dest.
   Inizialmente in path si memorizza il cammino dalla destinazione verso l'origine
   e poi lo si stampa nell'ordine giusto, cioè inverso.*/
void print_path(struct Cell dest) {
    int length_path = 0;        /*Variabile che misura la lunghezza del cammino*/
    int count_cells_rain = 0;   /*Variabile che conta quante celle bagnate ci sono lungo il cammino*/
    struct Cell curr = dest;
    int i;
    int prior_dist = 0;         /*Variabile usata per memorizzare la distanza precedente*/
    int nx = 0;
    int ny = 0;

    if (dist[dest.x][dest.y] == -1) {
        printf("-1 -1\n");
        return;
    }

    if(rain[dest.x][dest.y] == 0){
        count_cells_rain++;
    }

    /* Ripercorro il cammino dalla destinazione (dest) all'origine (start) e lo memorizzo in path*/
    while (prior_dist != -1){
        prior_dist = dist[curr.x][curr.y] - 1;

        for ( i = 0; i < 4; i++) {
            nx = curr.x + dir_x[i];
            ny = curr.y + dir_y[i];
            if (nx >= 0 && nx < N && ny >= 0 && ny < M && dist[nx][ny] == prior_dist && matrix[nx][ny] == 0) {
                path[length_path] = directions[i];
                length_path++;
                if(rain[nx][ny] == 0){           /*Se metto uguale ad 1 vuol dire che è asciutta perchè era vicino ad un palazzo*/
                    count_cells_rain++;
                }
                curr.x = nx;
                curr.y = ny;
                break;
            }
        }
    }

    printf("%d %d\n", dist[dest.x][dest.y]+1, count_cells_rain);

    /* Stampo il percorso nell'ordine giusto, cioè l'inverso di cio' che ho salvato in path*/
    for ( i = length_path - 1; i >= 0; i--) {
        printf("%c", path[i]);
    }
    printf("\n");
}


/*Funzione che calcola e contrassegna le celle colpite dalla pioggia, vengono messe a 1 se asciutte altrimenti a 0.
  Itero sulle righe e colonne della matrice matrix, se incontro una cella con numero 'c' diverso da 0, quindi che
  rappresenta un palazzo, per le 'c' caselle a dx del palazzo alto 'c' piani metto la corrispondente cella nella
  matrice rain uguale ad 1 per segnalare che è asciutta.*/
void generate_matrix_rain(){
    int row, col, i;
    for ( row = 0; row < N; row++) {
        for ( col = 0; col < M; col++) {
            if (matrix[row][col] != 0) {
                for ( i = 1; i <= matrix[row][col]; i++) {
                    if (col+i < M) {
                        rain[row][col+i] = 1;
                    }
                }
            }
        }
    }
}

/* Funzione che esegue la BFS.
   Si visita la matrice matrix usando l'algoritmo di visita in ampiezza (BFS)
   partendo dalla cella start (0,0). Vengono aggiornate le matrici: visited, in cui si
   fleggano a true le celle visitate, e dist, in cui per ogni cella visitata si
   inserisce la sua distanza da quella di start. Se la destinazione non è stata
   raggiunta nella matrice dist nella cella corrispondente alla destinazione,
   dist[dest.x][dest.y], si imposta il valore uguale a -1.*/
void bfs(struct Cell start, struct Cell dest) {
    int nx = 0;
    int ny = 0;
    bfsQueue[tail] = start;                    /* Aggiungo la cella di partenza alla coda*/
    tail++;
    visited[start.x][start.y] = true;
    dist[start.x][start.y] = 0;


    while (head != tail) {
        int i;
        struct Cell new_cell;
        struct Cell curr = bfsQueue[head];      /*estraiamo la cella corrente e head viene incrementato per puntare alla successiva*/
        head++;

        for ( i = 0; i < 4; i++) {              /*iteriamo sulle direzioni*/
            nx = curr.x + dir_x[i];
            ny = curr.y + dir_y[i];
            if (nx >= 0 && nx < N && ny >= 0 && ny < M && !visited[nx][ny]  && matrix[nx][ny] == 0) {
                visited[nx][ny] = true;
                dist[nx][ny] = dist[curr.x][curr.y] + 1;

                if (nx == dest.x && ny == dest.y) {
                    /*Sono arrivato alla cella di destinazione, esco dalla BFS*/
                    return;
                }

                new_cell.x = nx;
                new_cell.y = ny;
                bfsQueue[tail] = new_cell;        /*Se la cella adiacente non è la destinazione, viene aggiunta alla bfsQueue per essere esplorata successivamente*/
                tail++;
            }
        }
    }

    dist[dest.x][dest.y] = -1;
}



int main(int argc, char* argv[]) {
    struct Cell start = {0, 0};            /*coordinate cella di partenza*/
    struct Cell dest;
    int i;

    if (argc != 2) {
        fprintf(stderr, "Errore, numero di argomenti non valido. Utilizzo: %s <file di input>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    read_from_file(argv[1]);

    /*Allocazione dinamica della memoria per gli array*/
    bfsQueue = malloc(sizeof(struct Cell) * (N * M));
    path = malloc(sizeof(char) * (N * M) );

    /*coordinate cella di destinazione*/
    dest.x = N-1;
    dest.y = M-1;

    /*Setto i valori giusti nella matrice della pioggia, 0 per le caselle bagnate, 1 per le caselle asciutte */
    generate_matrix_rain();

    /*Uso BFS per calcolare le distanze minime*/
    bfs(start, dest);
    free(bfsQueue);

   /*Stampo a video la soluzione*/
    print_path(dest);

    /*Deallocazione della memoria*/
    for (i = 0; i < N; i++) {
        free(matrix[i]);
        free(rain[i]);
        free(visited[i]);
        free(dist[i]);
    }
    free(matrix);
    free(rain);
    free(visited);
    free(dist);
    free(path);

    return 0;
}
