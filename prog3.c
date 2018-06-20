#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_mutex_t sincronia;
pthread_mutex_t mut;

int loopInf = 1;
long int count = 0;
int vez = 0;

void *prodThread(void *arg);
void *consThread(void *arg);
void finish();

int main(){
    pthread_t cons[3], prod;
    signal(SIGTERM, finish);
    // Inicia Mutex
    if(pthread_mutex_init(&sincronia, NULL)){
        perror("ERRO Criar MUTEX");
        exit(1);
    }
    if(pthread_mutex_init(&mut, NULL)){
        perror("ERRO Criar MUTEX");
        exit(1);
    }
    // Cria threads
    for(int i = 0; i < 3; i++){
        if(pthread_create(&cons[i], NULL, consThread, NULL)){
            perror("CRIAR THREAD ERRO");
            exit(1);
        }
    }
    if(pthread_create(&prod, NULL, prodThread, NULL)){
        perror("CRIAR THREAD ERRO");
        exit(1);
    }
    // Espera threads
    for(int i = 0; i < 3; i++){
        if(pthread_join(cons[i], NULL)){
            perror("JOIN THREAD ERROR");
            exit(1);
        }
    }
    if(pthread_join(prod, NULL)){
        perror("JOIN THREAD ERROR");
        exit(1);
    }
    //destroy mutex
    if(pthread_mutex_destroy(&sincronia)){
        perror("ERRO DESTRUIR MUTEX");
        exit(1);
    }
    if(pthread_mutex_destroy(&mut)){
        perror("ERRO DESTRUIR MUTEX");
        exit(1);
    }

    return 0;
}
void *prodThread(void *arg){
    while(loopInf){
        pthread_mutex_lock(&sincronia);
        if(vez == 0){
            vez = 1;
            pthread_mutex_unlock(&sincronia);
            pthread_mutex_lock(&mut);
            count++;
            pthread_mutex_unlock(&mut);
        }
        else pthread_mutex_unlock(&sincronia);

    }
    pthread_exit(NULL);
}
void *consThread(void *arg){
    while(loopInf){
        pthread_mutex_lock(&sincronia);
        if(vez == 1){
            vez = 0;
            pthread_mutex_unlock(&sincronia);
            pthread_mutex_lock(&mut);
            printf("CONS RECEBE %li\n", count);
            pthread_mutex_unlock(&mut);
        }
        else pthread_mutex_unlock(&sincronia);
        
    }
    pthread_exit(NULL);
}
void finish(){
    loopInf = 0;
}