/*

Somatórias, seção crítica e reduções em OpenMP

Autor: Murilo Capozzi dos Santos
RA: 149425
Turma: I

*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define BOARD_SIZE 2048
#define NUM_THREADS 8
#define GENERATIONS 2000

long total_time = 0;

float **initGrid(){
    float **erased_grid;
    int i, j ;

    erased_grid = malloc(BOARD_SIZE * sizeof (float*));

    for(i = 0; i < BOARD_SIZE; i++)
        erased_grid[i] = malloc(BOARD_SIZE * sizeof(float));

    for(i = 0; i < BOARD_SIZE; i++){
        for(j = 0; j < BOARD_SIZE; j++)
            erased_grid[i][j] = 0.0;
    }

    erased_grid[1][2] = 1.0;
    erased_grid[2][3] = 1.0;
    erased_grid[3][1] = 1.0;
    erased_grid[3][2] = 1.0;
    erased_grid[3][3] = 1.0;

    // R-PENTOMINO
    erased_grid[10][31] = 1.0;
    erased_grid[10][32] = 1.0;
    erased_grid[11][30] = 1.0;
    erased_grid[11][31] = 1.0;
    erased_grid[12][31] = 1.0;

    return erased_grid;
}

void freeGrid(float **grid){
    int i;

    for(i = 0; i < BOARD_SIZE; i++)
        free(grid[i]);

    free(grid);
}

// Simular a matriz de borda infinita
int getCoord(int n){

    while(n % BOARD_SIZE < 0)
        n += BOARD_SIZE;

    return n % BOARD_SIZE;
}

int getNeighbors(float **grid, int i, int j){
    int k, l, quantity = 0;

    for(k = -1; k <= 1; k++){
        for(l = -1; l <= 1; l++){
            if(k == 0 && l == 0)
                continue;
            
            if(grid[getCoord(i + k)][getCoord(j + l)] > 0)
                quantity++; 
        }
    }

    return quantity;
}

float meanNeighbors(float **grid, int i, int j){
    int k, l;
    float sum = 0.0;

    for(k = -1; k <= 1; k++){
            for(l = -1; l <= 1; l++){
                if(k == 0 && l == 0)
                    continue;

                sum += grid[getCoord(i + k)][getCoord(j + l)];
            }
        }


    return (sum / 8.0);
}

float** gameOfLife(float **grid, float **newgrid){

    struct timeval start_time, end_time;

    int i, j, quantity = 0, quantityAlive = 0;
    float value;

    gettimeofday(&start_time, NULL);

#pragma omp parallel num_threads(NUM_THREADS) private(i,j,quantity,value) shared(grid, newgrid)

    for(i = 0; i < BOARD_SIZE; i++){

        #pragma omp for reduction(+:quantityAlive)

        for(j = 0; j < BOARD_SIZE; j++){
            quantity = getNeighbors(grid, i ,j);
            value = grid[i][j];

            // Regras para próximo estado
            if(value > 0 && (quantity == 2 || quantity == 3)){     // Se estão vivas e possuem 2 ou 3 vizinhos, permanecem
                newgrid[i][j] = 1.0;

                quantityAlive++;
            }
            else if(value == 0 && quantity == 3){                   // Se está morta e possui 3 vizinhos, renasce
                newgrid[i][j] = meanNeighbors(grid, i, j);
            
                quantityAlive++;
            }
            else
                newgrid[i][j] = 0.0;
        }
    }

#pragma omp barrier

    gettimeofday(&end_time, NULL);

    total_time += (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);

    printf("Quantidade de vivos: %d\n", quantityAlive);
    return newgrid;
}

int main(int argc, char **argv){
    int i, quantity;
    char c;

    float **actual = initGrid();
    float **new = initGrid();

    for(i = 0; i <= GENERATIONS; i++){

        float **temp = actual;
        actual = new;
        new = temp;

        new = gameOfLife(actual, new);

        //scanf("%c", &c);
        #pragma omp barrier
    }

    printf("Tempo total: %ld\n", total_time);
    freeGrid(actual);
    freeGrid(new);

    return 0;
}
