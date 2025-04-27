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
#define PORCOES_POR_ALUNO 2
#define N_COZINHEIROS 5

volatile int panelas[N_COZINHEIROS];
volatile int acabaram = 0;

int porcoes_comidas[N_ALUNOS];
int estado_aluno[N_ALUNOS]; // 0 = esperando, 1 = comendo, -1 = finalizado
int estado_cozinheiro[N_COZINHEIROS]; // 0 = dormindo, 1 = cozinhando

pthread_mutex_t mutex[N_COZINHEIROS];  // mutex para cada panela
sem_t panela_vazia;
sem_t panela_cheia;
pthread_mutex_t mutex_print;  

void print_estado_global() {
    pthread_mutex_lock(&mutex_print);
    printf("\n------------------------------\n");
    for (int i = 0; i < N_COZINHEIROS; i++) {
        printf("Panela %d: [", i);
        for (int j = 0; j < panelas[i]; j++) printf("\xF0\x9F\xA5\x98  ");
        printf("] (%d/%d)\n", panelas[i], N_PORCOES);
    }

    printf("Alunos:\n");
    for (int i = 0; i < N_ALUNOS; i++) {
        char *emoji;
        switch (estado_aluno[i]) {
            case 0: emoji = "\xF0\x9F\x8D\xBD"; break; 
            case 1: emoji = "\xF0\x9F\x98\xA4"; break;
            case -1: emoji = "\xF0\x9F\x98\x8A"; break; 
            default: emoji = " "; break;
        }
        printf("  [A%d] %s refeição %d/%d\n", i, emoji, porcoes_comidas[i], PORCOES_POR_ALUNO);
    }

    printf("Cozinheiros:\n");
    for (int i = 0; i < N_COZINHEIROS; i++) {
        char *emoji_coz = (estado_cozinheiro[i] == 1) ? "\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x8D\xB3" : "\xF0\x9F\x92\xA4";
        printf("Cozinheiro: %s %s\n", emoji_coz, (estado_cozinheiro[i] == 1) ? "cozinhando" : "dormindo");
    }
    printf("------------------------------\n\n");
    pthread_mutex_unlock(&mutex_print);
}

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
    estado_cozinheiro[id] = 1;
    char message[128];

    print_estado_global();

    snprintf(message, sizeof(message), "putServingsInPot %d %d", id, N_PORCOES);
    sendMessageToServer(message);

    sleep(1); 

    panelas[id] = N_PORCOES;
    estado_cozinheiro[id] = 0;

    print_estado_global();

    panelas[id] = N_PORCOES;
    
    sem_post(&panela_cheia);
}

void* f_cozinheiro(void *v) {
    int id = *(int*) v;
    int recarreguei = 0;
    estado_cozinheiro[id] = 0;
    char message[128];
    while (1) {
        sem_wait(&panela_vazia);
        snprintf(message, sizeof(message), "noFood %d", id);
        sendMessageToServer(message);

        if (acabaram) {
            printf("O cozinheiro %d está indo embora, ele recarregou sua panela %d vezes.\n", id, recarreguei);
            break;
        }
        putServingsInPot(id);
        recarreguei++;
        snprintf(message, sizeof(message), "returnChefToRest %d", id);
        sendMessageToServer(message);
        sleep(15); 
    }
    return NULL;
}

void* f_aluno(void *v) {
    int id = *(int*) v;
    porcoes_comidas[id] = 0;
    char message[128];

    print_estado_global();

    while (porcoes_comidas[id] < PORCOES_POR_ALUNO) {
        int conseguiu_comer = 0;
        for (int i = 0; i < N_COZINHEIROS; i++) {
            pthread_mutex_lock(&mutex[i]);
            if (panelas[i] > 0) {
                snprintf(message, sizeof(message), "getFood %d %d", id, i);
                sendMessageToServer(message);
                sleep(6);
                panelas[i]--;
                porcoes_comidas[id]++;

                estado_aluno[id] = 1;
                conseguiu_comer = 1;
                pthread_mutex_unlock(&mutex[i]);
                snprintf(message, sizeof(message), "returnCustomerToRest %d", id);
                sendMessageToServer(message);
                sleep(3);
                break;
            }
            pthread_mutex_unlock(&mutex[i]);
        }

        if (!conseguiu_comer) {
            printf("Aluno %d: Acabou a feijoada! Vou chamar os cozinheiros.\n", id); //seria bom 
            sem_post(&panela_vazia);
            sem_wait(&panela_cheia);
        } else {
            sleep(rand() % 3 + 1); 
        }
    }
    
    estado_aluno[id] = -1;
    print_estado_global();
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
        pthread_mutex_init(&mutex[i], NULL);
        panelas[i] = 0;
        estado_cozinheiro[i] = 0;
    }

    sem_init(&panela_vazia, 0, 0);
    sem_init(&panela_cheia, 0, 0);

    snprintf(message, sizeof(message), "init %d %d %d %d", N_PORCOES, N_ALUNOS, N_COZINHEIROS, PORCOES_POR_ALUNO);
    sendMessageToServer(message);

    sleep(3); // Esperando carregar a tela

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
        pthread_mutex_destroy(&mutex[i]);
    }

    pthread_mutex_destroy(&mutex_print);
    sem_destroy(&panela_vazia);
    sem_destroy(&panela_cheia);

    printf("Muçamos! Todos os alunos comeram %d vezes.\n", PORCOES_POR_ALUNO);

    snprintf(message, sizeof(message), "end");
    sendMessageToServer(message);

    return 0;
}
