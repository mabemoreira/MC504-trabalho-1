#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define N_PORCOES 3
#define N_ALUNOS 7
#define PORCOES_POR_ALUNO 10
#define N_COZINHEIROS 5

volatile int panelas[N_COZINHEIROS]; // guarda quanto tem na panela i
volatile int acabaram = 0;

sem_t mutex[N_COZINHEIROS];  // q tb é o número de panelas, já que cada cozinheiro enche a sua       
sem_t panela_vazia;                
sem_t panela_cheia;      

void sendMessageToServer(const char* message) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erro ao criar socket");
        return;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(53002);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
        perror("Erro ao configurar endereço do servidor");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Erro ao conectar ao servidor");
        close(sock);
        return;
    }

    send(sock, message, strlen(message), 0);
    close(sock);
}

void putServingsInPot(int id) {
    sleep(1); 
    panelas[id] = N_PORCOES;
    printf("O cozinheiro %d colocou %d porções na sua panela\n", id, N_PORCOES);
    sem_post(&panela_cheia);
}

void* f_cozinheiro(void *v) {
    int id = *(int*) v;
    int recarreguei = 0;
    while (1) {
        sem_wait(&panela_vazia);
        if (acabaram) {
            printf("O cozinheiro %d está indo embora, ele recarregou sua panela %d vezes.\n", id, recarreguei);
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
        int conseguiu_comer = 0;
        for (int i = 0; i < N_COZINHEIROS; i++) {
            sem_wait(&mutex[i]);
            if (panelas[i] > 0) {
                panelas[i]--;
                porcoes_comidas++;
                printf("Aluno %d: pegou da panela %d. (Refeição %d/%d) Porções restantes nessa panela: %d\n",
                       id, i, porcoes_comidas, PORCOES_POR_ALUNO, panelas[i]);
                conseguiu_comer = 1;
                sem_post(&mutex[i]);
                break;
            }
            sem_post(&mutex[i]);
        }

        if (!conseguiu_comer) {
            printf("Aluno %d: Acabou a feijoada! Vou chamar os cozinheiros.\n", id); //seria bom 
            sem_post(&panela_vazia);
            sem_wait(&panela_cheia);
        } else {
            sleep(rand() % 3 + 1); 
        }
    }
    
    printf("Aluno %d: terminei de comer %d vezes.\n", id, PORCOES_POR_ALUNO);
    return NULL;
}

int main() {
    char message[128];

    if (N_PORCOES <= 0){
        fprintf(stderr, "Como assim você não vai repor a comida em dia de feijoada? N_PORCOES deve ser maior que 0!\n");
        exit(EXIT_FAILURE);
    }
    pthread_t threads_alunos[N_ALUNOS], threads_cozinheiros[N_COZINHEIROS];
    int ids_alunos[N_ALUNOS];
    int ids_cozinheiros[N_COZINHEIROS];

   
    for (int i = 0; i < N_COZINHEIROS; i++) {
        sem_init(&mutex[i], 0, 1);
        panelas[i] = 0;
    }

    sem_init(&panela_vazia, 0, 0);
    sem_init(&panela_cheia, 0, 0);

    snprintf(message, sizeof(message), "init %d %d %d %d", N_PORCOES, N_ALUNOS, N_COZINHEIROS, PORCOES_POR_ALUNO);
    sendMessageToServer(message);

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
        sem_destroy(&mutex[i]);
    }

    sem_destroy(&panela_vazia);
    sem_destroy(&panela_cheia);

    printf("Muçamos! Todos os alunos comeram %d vezes.\n", PORCOES_POR_ALUNO);
    return 0;
}
