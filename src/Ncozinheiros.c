#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N_PORCOES 3
#define N_ALUNOS 7
#define PORCOES_POR_ALUNO 10
#define N_COZINHEIROS 2

volatile int porcoes = 0;
volatile int acabaram = 0;

int porcoes_comidas[N_ALUNOS];
int estado_aluno[N_ALUNOS]; // 0 = esperando, 1 = comendo, -1 = finalizado
int estado_cozinheiro[N_COZINHEIROS]; // 0 = dormindo, 1 = cozinhando

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
    for(int i = 0; i < N_COZINHEIROS; i++) {
        char *emoji_coz = (estado_cozinheiro[i] == 1) ? "\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x8D\xB3" : "\xF0\x9F\x92\xA4"; // 👨‍🍳 ou 😴
        printf("Cozinheiro: %s %s\n", emoji_coz, (estado_cozinheiro[i] == 1) ? "cozinhando" : "dormindo");
    }
    printf("------------------------------\n\n");
    pthread_mutex_unlock(&mutex_print);
}

void putServingsInPot(int n) {
    // cozinheiro é colocado como trabalhando, e imprime o estado global. 
    //depois de repor a panela, imprime novamente e avisa aos alunos que está cheia
    estado_cozinheiro[n] = 1;
    print_estado_global();
    sleep(1);
    porcoes += N_PORCOES;
    estado_cozinheiro[n] = 0;
    print_estado_global();
    sem_post(&panela_cheia);
}

void* f_cozinheiro(void *v) {
    // espera até receber sinal que a panela está vazia e chama putServingsinPot para recarregá-la
    int id = *(int*) v;
    int recarreguei = 0;
    estado_cozinheiro[id] = 0;
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
    porcoes_comidas[id] = 0;
    estado_aluno[id] = 0;
    print_estado_global();
// enquanto não comer a quantidade estipulada, tenta pegar o lock da panela
// quando consegue, checa se ela está vazia. Se sim, espera o cozinheiro encher 
    while (porcoes_comidas[id] < PORCOES_POR_ALUNO) {
        estado_aluno[id] = 0;
        print_estado_global();
        pthread_mutex_lock(&mutex);

        if (porcoes == 0) {
            sem_post(&panela_vazia);
            sem_wait(&panela_cheia);
        }

        porcoes--;
        porcoes_comidas[id]++;
        estado_aluno[id] = 1;
        print_estado_global();
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3 + 1);
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

    pthread_t threads_alunos[N_ALUNOS], threads_cozinheiros[N_COZINHEIROS];
    int ids_alunos[N_ALUNOS];
    int ids_cozinheiros[N_COZINHEIROS];

    // Inicializa os mutexes
    pthread_mutex_init(&mutex_print, NULL);
    pthread_mutex_init(&mutex, NULL);
    
    // Inicializa os semáforos
    sem_init(&panela_vazia, 0, 0);
    sem_init(&panela_cheia, 0, 0);

    for (int i = 0; i < N_COZINHEIROS; i++) {
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

    for (int i = 0; i < N_COZINHEIROS; i++) {
        sem_post(&panela_vazia);
    }

    for (int i = 0; i < N_COZINHEIROS; i++) {
        pthread_join(threads_cozinheiros[i], NULL);
    }

    // Destroi os mutexes
    pthread_mutex_destroy(&mutex_print);
    pthread_mutex_destroy(&mutex);
    
    // Destroi os semáforos
    sem_destroy(&panela_vazia);
    sem_destroy(&panela_cheia);

    return 0;
}