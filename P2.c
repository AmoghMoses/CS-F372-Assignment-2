#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int k;

long long int m3[1001][1001];
int m, n, p;
struct thread_attr
{
    int thread_num;
};

int m1;
int m2;
int shmid5;
int shmid6;
                   
int (*str1)[1000];
int (*str2)[1000];
int (*str5)[1000];
int (*str6)[1000];

void *runner(void *arg){
    struct thread_attr* th  = (struct thread_attr*)arg;
    
    int total = m * p;
    int number = (m * p) / k;
    int left = (m * p) % k;

    int start, end;
   
    start = th->thread_num == 0 ? number*th->thread_num:number*th->thread_num+left;    
    
    end = number * (th->thread_num + 1) + left;
    
    for(int num1 = start; num1 < end; num1++){
        int i = num1 % m;
        int j = num1 / m;
        int tempsum = 0;

        for(int l = 0; l < n; l++){
            tempsum += str1[i][l] * str2[j][l];
        }
        m3[i][j] = tempsum;  
    }
}

int main(){
    // Manually Giving number of threads
    k = 1;
    
    key_t key1 = ftok("/", 67590);
    key_t key2 = ftok("/", 250000);
    key_t key3 = ftok("/", 2345);

    m1 = shmget(key1, sizeof(int[1000][1000]), 0666|IPC_CREAT);
    m2 = shmget(key2, sizeof(int[1000][1000]), 0666|IPC_CREAT);
    int shmid3 = shmget(key3, sizeof(int[3]), 0666|IPC_CREAT);

    key_t key4 = ftok("/", 2345234);
    int shmid4 = shmget(key4, sizeof(int[2]), 0666|IPC_CREAT);
    int *str4 = shmat(shmid4, (void*)0, 0);
    str4[1] = 0;

    key_t key5 = ftok("/", 23345);
    shmid5 = shmget(key5, sizeof(int[1000][1000]), 0666|IPC_CREAT);
    str5 = shmat(shmid5, (void*)0, 0);

    key_t key6 = ftok("/", 234980);
    shmid6 = shmget(key6, sizeof(int[1000][1000]), 0666|IPC_CREAT);
    str6 = shmat(shmid6, (void*)0, 0);

    str1 = shmat(m1, (void*)0, 0);
    str2 = shmat(m2, (void*)0, 0);
    int *str3 = shmat(shmid3, (void*)0, 0);

    m = str3[0];
    n = str3[1];
    p = str3[2];
 
    pthread_t threads2[k];
    struct thread_attr jumps1[k];

    for(int i = 0; i < k; i++){
        jumps1[i].thread_num = i;
    }
    for(int i = 0; i < k; i++){
        pthread_create(&threads2[i], NULL, runner, (void*)&jumps1[i]);
    }
    for(int i = 0; i < k; i++){
        pthread_join(threads2[i], NULL);
    }

    FILE *fptr;
    fptr = fopen("out.txt","w");
    for(int i = 0; i < m; i++){
        for(int j = 0; j < p; j++) {
            fprintf(fptr,"%lld", m3[i][j]);
            fprintf(fptr," ");
        }
        fprintf(fptr,"\n");
    }
    // printf("\n");
    shmdt(str1);
    shmdt(str2);    
    
    fclose(fptr);
    str4[1] = 1;
    // printf("str4 in p2 is %d ", str4[1]);

}
