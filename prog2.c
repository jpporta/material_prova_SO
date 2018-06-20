#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>

#define MSG_KEY 34567

typedef struct{
    long mtype;
    char mtext[8];
}msgbuf_t;

void prod();
void cons();
void finish();

int loopInf = 1;
int idMsg;

int main(){
    pid_t fcons, fprod;
    signal(SIGTERM, finish);

    // Menssege Queue
    key_t msgKey = MSG_KEY;

    if((idMsg = msgget(msgKey, IPC_CREAT | 0666)) == -1){
        perror("MSGGET ERROR");
        exit(1);
    }

    fcons = fork();
    if(fcons == 0){
        cons();
        exit(0);
    }
    fprod = fork();
    if(fprod == 0){
        prod();
        exit(0);
    }

    while(loopInf);

    kill(fcons, SIGTERM);
    kill(fprod, SIGTERM);

    if(msgctl(idMsg, IPC_RMID, NULL) == -1){
        perror("Erros MSGCTL");
        exit(1);
    }

    return 0;
}
void cons(){
    msgbuf_t msg;
    long int *cont = (long int *)(msg.mtext);
    size_t tamMsg = sizeof(long int);
    while(1){
        if(msgrcv(idMsg, (msgbuf_t *)&msg, tamMsg, 1, 0) == -1){
            perror("Erro MSGRCV");
            exit(1);
        }
        printf("Cons recebe %li\n", *cont);
    }
}
void prod(){
    msgbuf_t msg;
    long int contador = 0;
    long int *cont = (long int *)(msg.mtext);
    size_t tamMsg = sizeof(long int);

    while(1){
        *cont = contador;
        msg.mtype = 1;

        if(msgsnd(idMsg, (msgbuf_t *)&msg, tamMsg, 0) == -1){
            perror("ERRO MSGRCV");
            exit(1);
        }

        contador++;
    }
}
void finish(){
    loopInf = 0;
}