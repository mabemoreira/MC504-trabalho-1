#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N_PORCOES 3
#define N_ALUNOS 7
#define PORCOES_POR_ALUNO 2
#define N_COZINHEIROS 2

volatile int porcoes = 0;
volatile int acabaram = 0; // é um bool 

sem_t mutex;  // avisa se pode pegar comida       
sem_t panela_vazia;  // avisa se a panela ta vazia
sem_t panela_cheia;  // avisa se a panela ta cheia 


void putServingsInPot(int n) {
    sleep(1); // tempo de cozinhar
    porcoes += N_PORCOES; 
    printf("O cozinheiro %d colocou %d porções na panela\n", n, N_PORCOES);
    sem_post(&panela_cheia);
}

void* f_cozinheiro(void *v) {
    int id = *(int*) v;
    int recarreguei = 0;
    while (1) {
        sem_wait(&panela_vazia);
        if (acabaram) {
            printf("O cozinheiro %d está indo embora, ele recarregou a panela %d vezes.\n", id, recarreguei);
            break;
        }
        putServingsInPot(id);
        recarreguei++;
    }
    return NULL;
}

void* f_aluno(void *v) {
    int id = *(int*) v;
    int porcoes_comidas = 0;
    while (porcoes_comidas < PORCOES_POR_ALUNO) {
        sem_wait(&mutex); // esperar minha vez na fila
        
        if (porcoes == 0) {
            printf("Aluno %d: acabou a feijoada! Vou chamar o cozinheiro.\n", id);
            sem_post(&panela_vazia);
            sem_wait(&panela_cheia);
        }
      
        porcoes--; 
        porcoes_comidas++;
        printf("Aluno %d: pegou uma porção. (Refeição %d/%d) Porções restantes: %d\n", id, porcoes_comidas, PORCOES_POR_ALUNO, porcoes);
        sem_post(&mutex); // saindo da fila
        sleep(rand() % 3 + 1); // comendo
    }
    printf("Aluno %d: terminei de comer %d vezes.\n", id, PORCOES_POR_ALUNO);
    return NULL;
}

int main() {
    if (N_PORCOES <= 0){
        fprintf(stderr, "Como assim você não vai repor a comida em dia de feijoada? N_PORCOES deve ser maior que 0!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads_alunos[N_ALUNOS], threads_cozinheiros[N_COZINHEIROS];
    int ids_alunos[N_ALUNOS];
    int ids_cozinheiros[N_COZINHEIROS];

    sem_init(&mutex, 0, 1);
    sem_init(&panela_vazia, 0, 0);
    sem_init(&panela_cheia, 0, 0);

    for(int i = 0; i < N_COZINHEIROS; i++){
        ids_cozinheiros[i] = i;
        pthread_create(&threads_cozinheiros[i], NULL, f_cozinheiro, &ids_cozinheiros[i]);
    }
    


    for (int i = 0; i < N_ALUNOS; i++) {
        ids_alunos[i] = i;
        pthread_create(&threads_alunos[i], NULL, f_aluno, &ids_alunos[i]);
    }

   
    for (int i = 0; i < N_ALUNOS; i++) {
        pthread_join(threads_alunos[i], NULL);
    }

   
    acabaram = 1;

    for(int i = 0; i < N_COZINHEIROS; i++){
        sem_post(&panela_vazia);
    }
   
    
    for(int i = 0; i < N_COZINHEIROS; i++){
        pthread_join(threads_cozinheiros[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&panela_vazia);
    sem_destroy(&panela_cheia);

    printf("Muçamos! Todos os alunos comeram %d vezes.\n", PORCOES_POR_ALUNO);
    return 0;
}
