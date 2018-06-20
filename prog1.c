#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>

#define SHM_KEY 9876
#define SEM_KEY 6789

int idSemaforo;
int idSHM;
int loopInf = 1;

struct sembuf semopAbre[1];
struct sembuf semopFecha[1];

long int *shmAddress;

// Funcoes
void prod();
void cons();
void terminandoProg();

int main(){

    signal(SIGINT, terminandoProg);
    // Semaforo
    semopAbre[0].sem_num = 0;
    semopAbre[0].sem_op = 1;
    semopAbre[0].sem_flg = 0;

    semopFecha[0].sem_num = 0;
    semopFecha[0].sem_op = -1;
    semopFecha[0].sem_flg = 0;

    if((idSemaforo = semget(SEM_KEY, 1, IPC_CREAT | 0666)) == -1){
        perror("semget error");
        exit(-1);
    }
    if(semop(idSemaforo, semopAbre, 1) == -1){
        perror("abre1 sem error");
        exit(-1);
    }
     // Shared Memory
    if((idSHM = shmget(SHM_KEY, sizeof(long int), IPC_CREAT | 0666)) == -1){
        perror("shmget error");
        exit(-1);
    }
    if((shmAddress = (long int *)shmat(idSHM, NULL, 0)) == (long int *)-1 ){
        perror("shmat error");
        exit(-1);
    }

    //main 

    *shmAddress = 0;
    pid_t rtn;

    rtn = fork();
    if(rtn == 0){
        prod();
        exit(0);
    }
    rtn = fork();
    if(rtn == 0){
        cons();
        exit(0);
    }

    while(loopInf);

    if( shmctl(idSHM, IPC_RMID, NULL) != 0 ) {
        perror("Remove shm");
        exit(-1);
    }
    if( semctl(idSemaforo, 0, IPC_RMID, 0) != 0 ) {
        perror("Remove sem");
        exit(-1);
    }
    
    return 0;
}
void prod(){
    while(1){
        // Fecha semaforo
        if(semop(idSemaforo, semopFecha, 1) == -1){
            perror("abre3 sem error");
            exit(-1);
        }
        // Regia critica
        *shmAddress = *shmAddress + 1;
        // Abre semaforo
        if(semop(idSemaforo, semopAbre, 1) == -1){
            perror("fecha sem error");
            exit(-1);
        }
    }
}

void cons(){
    while(1){
        // Fecha semaforo
        if(semop(idSemaforo, semopFecha, 1) == -1){
            perror("abre5 sem error");
            exit(-1);
        }
        // Regia critica
        printf("Cons recebeu %li\n", *shmAddress);
        // Abre semaforo
        if(semop(idSemaforo, semopAbre, 1) == -1){
            perror("fecha sem error");
            exit(-1);
        }
    }
}

void terminandoProg(){
    loopInf = 0;
}