#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
          
struct thread_attr{
    char *fi;
    int start_i;
    int end_i;
    int index;
    int matrix;
    //int* matrix[1000];
    int wl;
    int rem;
    int cols;
};
            
int m, n, p;
char inputFile1[50]; // Size
char inputFile2[50];
char outputFile[50];
int rem1, rem2;
int k;
    
int shmid1;
int shmid2;
int shmid5;
int shmid6;        
            
int (*str1)[1000];
int (*str2)[1000];
int (*str5)[1000];
int (*str6)[1000];
    
void *runner(void *arg){
    int i;
    int c;
    char d;
    struct thread_attr* th  = (struct thread_attr*)arg;
    FILE* file = fopen(th -> fi, "r");
    fseek(file , th -> start_i, SEEK_SET);
    
    int counter = 0;
    int index = th -> index;
    int x, y;
    x = (th -> index * th -> wl) / th->cols;
    y = (th -> index * th -> wl) % th->cols;
    int counter1 = th -> wl;
        
    if(th -> index == k - 1){
        counter1 += th -> rem;
    }
        
    for(i = 0; i < counter1; i++){
        fscanf(file, "%d", &c);

        if(th->matrix == 1){
            str1[x][y] = c;
            str5[x][y] = 1;
        }
         else if(th->matrix == 2){
            str2[x][y] = c;
            str6[x][y] = 1;
            }
            
        y++;
            
        if(y == th -> cols){
            x++;
            y = 0;
        }
    }
    return NULL;
}
            
int main(int argc, char** argv){
    if (argc < 7){
        printf("Usage: ./groupX_assignment2.out i j k in1.txt in2.txt out.txt");
    	exit(-1);
    }
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    p = atoi(argv[3]);

    strcpy(inputFile1, argv[4]);
    strcpy(inputFile2, argv[5]);
    strcpy(outputFile, argv[6]);
        
    key_t key1 = ftok("/", 67590);
    key_t key2 = ftok("/", 250000);
    key_t key3 = ftok("/", 2345);
        
    if(key1 == -1 || key2 == -1 || key3 == -1 ){
        printf("Error in generation");
        return 0;
    }
           
    shmid1 = shmget(key1, sizeof(int[1000][1000]), 0666|IPC_CREAT);
    shmid2 = shmget(key2, sizeof(int[1000][1000]), 0666|IPC_CREAT);
            
    int shmid3 = shmget(key3, sizeof(int[3]), 0666|IPC_CREAT);
            
    str1 = shmat(shmid1, (void*)0, 0);
    str2 = shmat(shmid2, (void*)0, 0);

    int *str3 = shmat(shmid3, (void*)0, 0);
    
    key_t key4 = ftok("/", 2345234);
    int shmid4 = shmget(key4, sizeof(int[2]), 0666|IPC_CREAT);
    int *str4 = shmat(shmid4, (void*)0, 0);
    
    key_t key5 = ftok("/", 23345);
    shmid5 = shmget(key5, sizeof(int[1000][1000]), 0666|IPC_CREAT);
    str5 = shmat(shmid5, (void*)0, 0);

    key_t key6 = ftok("/", 234980);
    shmid6 = shmget(key6, sizeof(int[1000][1000]), 0666|IPC_CREAT);
    str6 = shmat(shmid6, (void*)0, 0);   

    key_t keyS = ftok("/", 234523244);
    int shmidS = shmget(keyS, sizeof(int), IPC_CREAT|0666);
    int *strS = shmat(shmidS, 0, 0);
    

    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            str5[i][j] = 0;
        }
    }

    for(int i = 0; i < p; i++){
        for(int j = 0; j < n; j++){
            str6[i][j] = 0;
        }
    }
    // printf("Initialisation finished");
    *strS = 1;
        
    str4[0] = 0;
    str3[0] = m;
    str3[1] = n;
    str3[2] = p;
        
    // Manually give arbitrary number of threads       
    k = 1;

    pid_t pid = fork();
    if(pid == 0){
        execlp("python3","python3","transpose.py", inputFile2, NULL);
    }
    else{
        wait(NULL);
        FILE *fp1 = fopen(inputFile1, "r");
        FILE *fp2 = fopen(inputFile2, "r");
            
        int wl1 = (m * n) / k;
        rem1 = (m * n) % k;
        
        int wl2 = (n * p) / k;
        rem2 = (n * p) % k;
        
        pthread_t threads1[k];
        struct thread_attr jumps1[k];
        jumps1[0].start_i = 0;
            
        pthread_t threads2[k];
        struct thread_attr jumps2[k];
        jumps2[0].start_i = 0;
            
        int a1 = 1;
        int i1;
            
        int main_counter1 = 0;
        char ch1;
        int count1 = 0;
        
        while (a1 < k){
            ch1 = fgetc (fp1);
            //Each character of the file is read and stored in the character file.  
            main_counter1++;
            //printf("%c %d ", ch, main_counter);
            if (ch1 == EOF)
                break;
                
            if(ch1 == ' ' || ch1 == '\n'){
                count1++;
            }
            if(count1 == wl1){
                jumps1[a1].start_i = main_counter1;
                //printf("jump updated");
                count1 = 0;
                a1++;
            }
        }  
         
        for(int i1 = 0; i1 < k; i1++){
            jumps1[i1].wl = wl1;
            jumps1[i1].end_i = jumps1[i1 + 1].start_i;
            jumps1[i1].fi = inputFile1;
            jumps1[i1].rem = rem1;
            jumps1[i1].cols = n;
            jumps1[i1].matrix = 1;
        }
        
        fseek(fp1, 0, SEEK_END);
            
        // For Matrix 2:
        int a2 = 1;    
        int i2;
        int main_counter2 = 0;
            
        int count2 = 0;
        char ch2;
        
        while (a2 < k){
            ch2 = fgetc (fp2);
            //Each character of the file is read and stored in the character file.  
            main_counter2++;
            //printf("%c %d ", ch, main_counter);
            if (ch2 == EOF)
                break;
                 
            if(ch2 == ' ' || ch2 == '\n'){
                count2++;
            }
            if(count2 == wl2){
                jumps2[a2].start_i = main_counter2;
                    //printf("jump updated");
                    count2 = 0;
                    a2++;
            }
        }  
         
        for(int i2 = 0; i2 < k; i2++){
            jumps2[i2].wl = wl2;
            jumps2[i2].end_i = jumps2[i2 + 1].start_i;
            jumps2[i2].fi = inputFile2;
            jumps2[i2].rem = rem2;
            jumps2[i2].cols = n;
            jumps2[i2].matrix = 2;
        }
        
        fseek(fp2, 0, SEEK_END);
    
        for(int i = 0; i < k; i++){
            jumps1[i].index = i;
            jumps2[i].index = i;  
            pthread_create(&threads1[i], NULL, runner, (void*)&jumps1[i]);
            pthread_create(&threads2[i], NULL, runner, (void*)&jumps2[i]);
        }
         
        for(int i = 0; i < k; i++){
            pthread_join(threads1[i], NULL);
            pthread_join(threads2[i], NULL);
        }
            
        for(int i = 0; i < m; i++){
            for(int j = 0; j < n; j++){
                printf("%d ", str1[i][j]);
            }
            printf("\n");
        }
            
        printf("\n");
            
        for(int i = 0; i < p; i++){
            for(int j = 0; j < n; j++){
                printf("%d ", str2[i][j]);
            }
            printf("\n");
        }
            
        str4[0] = 1;
        // printf("str4 is %d ", str4[0]);    
    }
    return 0;
}
     