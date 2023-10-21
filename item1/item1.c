/*

Seção Crítica por espera ocupada

Autor: Murilo Capozzi dos Santos
RA: 149425
Turma: I

*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SOMA 100000

int soma = 0, request = 0, respond = 0;

void cliente(int t_id){
    int i = 0;
    while(i != MAX_SOMA){

        while(respond != t_id)
            request = t_id;

        // Inicio SC
        printf("%d", t_id);
        int local = soma;

        soma = local + 1;

        //sleep(rand()%2);

        if(soma != local + 1){
            printf("\nERRO! EXCLUSAO MUTUA NAO GARANTIDA\n");
            exit(-1);
        }

        // Fim SC
        i++;
        printf("%d ", t_id);
        respond = 0;
    }
    
    printf("\n\nThread %d completou %d somas\n\n", t_id, i);
}

void servidor(int num_threads){
    while(soma != MAX_SOMA*num_threads){
        while(request == 0);

        respond = request;

        while(respond != 0);

        request = 0;
    }

    printf("\nSoma = %d\n", soma);
}

int main(int argc, char *argv[]){
    int i, num_threads = atoi(argv[1]);
    
#pragma omp parallel num_threads(num_threads+1)
{
    if(omp_get_thread_num() == 0){
        printf("%d threads\n", omp_get_num_threads()-1);
        servidor(omp_get_num_threads()-1);
    }
    else
        cliente(omp_get_thread_num());
}
    
    return 0;
}
