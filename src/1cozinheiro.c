#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N_PORCOES 7
#define N_ALUNOS 10
#define PORCOES_POR_ALUNO 5

volatile int porcoes = 0;
volatile int acabaram = 0; // é um bool

int porcoes_comidas[N_ALUNOS];
int estado_aluno[N_ALUNOS]; // 0 = esperando, 1 = comendo, -1 = finalizado
int estado_cozinheiro = 0; // 0 = dormindo, 1 = cozinhando

pthread_mutex_t mutex;  
sem_t panela_vazia;     // avisa se a panela ta vazia
sem_t panela_cheia;     // avisa se a panela ta cheia
pthread_mutex_t mutex_print; 

void print_estado_global() {
    // imprime o estado global da aplicação. Apesar do mutex de impressão só ser chamado nessa função, ela é chamada
    // por múltiplas threads, então ele se faz necessário
    pthread_mutex_lock(&mutex_print);
    printf("\n------------------------------\n");
    printf("Panela: [");
    for (int i = 0; i < porcoes; i++) printf("\xF0\x9F\xA5\x98  "); 
    printf("] (%d/%d porções)\n", porcoes, N_PORCOES);

    printf("Alunos:\n");
    for (int i = 0; i < N_ALUNOS; i++) {
        char *emoji;
        switch (estado_aluno[i]) {
            case 0: emoji = "\xF0\x9F\x98\xA4"; break; 
            case 1: emoji = "\xF0\x9F\x8D\xBD"; break;
            case -1: emoji = "\xF0\x9F\x98\x8A"; break; 
            default: emoji = " "; break;
        }
        printf("  [A%d] %s refeição %d/%d\n", i, emoji, porcoes_comidas[i], PORCOES_POR_ALUNO);
    }
    
    char *emoji_coz = (estado_cozinheiro == 1) ? "\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x8D\xB3" : "\xF0\x9F\x92\xA4"; 
    printf("Cozinheiro: %s %s\n", emoji_coz, (estado_cozinheiro == 1) ? "cozinhando" : "dormindo");
    printf("------------------------------\n\n");
    pthread_mutex_unlock(&mutex_print);
}

void putServingsInPot() {
    estado_cozinheiro = 1;
    print_estado_global();
    sleep(1); // tempo de cozinhar
    porcoes = N_PORCOES; // ele sempre repoe todas 
    estado_cozinheiro = 0;
    print_estado_global();
    sem_post(&panela_cheia);
}

void* f_cozinheiro() {
    while (1) {
        sem_wait(&panela_vazia);
        if (acabaram) {
            break;
        };
        putServingsInPot();
    }
    return NULL;
}

void* f_aluno(void *v) {
    int id = *(int*) v;
    porcoes_comidas[id] = 0;
    estado_aluno[id] = 0;
    print_estado_global();

    while (porcoes_comidas[id] < PORCOES_POR_ALUNO) {
        estado_aluno[id] = 0;
        print_estado_global();
        pthread_mutex_lock(&mutex); // esperar minha vez na fila

        if (porcoes == 0) {
            sem_post(&panela_vazia);
            sem_wait(&panela_cheia);
        }

        porcoes--;
        porcoes_comidas[id]++;
        estado_aluno[id] = 1;
        print_estado_global();
        pthread_mutex_unlock(&mutex); // saindo da fila

        sleep(rand() % 3 + 1); // comendo
    }

    estado_aluno[id] = -1;
    print_estado_global();
    return NULL;
}

int main() {
    if (N_PORCOES <= 0){
        fprintf(stderr, "Como assim você não vai repor a comida em dia de feijoada? N_PORCOES deve ser maior que 0!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads_alunos[N_ALUNOS], thread_cozinheiro;
    int ids[N_ALUNOS];
    
    // Inicializa os mutexes
    pthread_mutex_init(&mutex_print, NULL);
    pthread_mutex_init(&mutex, NULL);
    
    // Inicializa os semáforos
    sem_init(&panela_vazia, 0, 0);
    sem_init(&panela_cheia, 0, 0);

    pthread_create(&thread_cozinheiro, NULL, f_cozinheiro, NULL);

    for (int i = 0; i < N_ALUNOS; i++) {
        ids[i] = i;
        pthread_create(&threads_alunos[i], NULL, f_aluno, &ids[i]);
    }

    for (int i = 0; i < N_ALUNOS; i++) {
        pthread_join(threads_alunos[i], NULL);
    }

    acabaram = 1;
    sem_post(&panela_vazia);
    pthread_join(thread_cozinheiro, NULL);
    
    // Destroi os mutexes
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_print);
    
    // Destroi os semáforos
    sem_destroy(&panela_vazia);
    sem_destroy(&panela_cheia);

    return 0;
}