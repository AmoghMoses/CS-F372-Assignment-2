#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>

pid_t pid1, pid2, pid3;
char inputFile1[50]; 
char inputFile2[50];
char outputFile[50]; 
int timeQuantum = 0.002;
int main(int argc, char** argv)
{
    char m, n, p;
    if (argc < 7) {
        printf("Usage: ./groupX_assignment2.out i j k in1.txt in2.txt out.txt");
    	exit(-1);
    }

    m = *argv[1];
    n = *argv[2];
    p = *argv[3];

    strcpy(inputFile1, argv[4]);
    strcpy(inputFile2, argv[5]);
    strcpy(outputFile, argv[6]);

    key_t key1 = ftok("/", 2345234);
    int shmid1 = shmget(key1, sizeof(int[2]), IPC_CREAT|0666);
    int *str1 = shmat(shmid1, 0, 0);

    key_t keyS = ftok("/", 234523244);
    int shmidS = shmget(keyS, sizeof(int), IPC_CREAT|0666);
    int *strS = shmat(shmidS, 0, 0);
    
    str1[0] = 0; 
    str1[1] = 0;  
    pid1 = fork(); 
    if(pid1 == 0){
        pid1 = getpid();    // getting pid of the first child
        kill(pid1, SIGSTOP); // printf("First p stopped\n");
        execlp("./P1.out", "P1.out", argv[1], argv[2], argv[3], inputFile1, inputFile2, outputFile, NULL);//Add arguments as argv[1] ..... 
    }
    else{
        pid3 = getpid(); //pid of the second child
        pid2 = fork();   
        if(pid2 == 0){ //P2
            pid2 = getpid();
            kill(pid2, SIGSTOP);
            execlp("./P2.out", "P2.out", NULL); //add arguments   
        }
        else{ //Parent Scheduler

            int local = pid2;

            sleep(1);

            //clock_t tic = clock();
            //clock_t wait = clock();
            // printf("First process stoopped for after in\n");
            kill(pid1, SIGCONT);
            while(!(*strS)){
                // printf("l");
                continue;
                
            }

            kill(pid1, SIGSTOP);
            while(!str1[0] && !str1[1])
            {
                if(local == pid2)
                {
                    local = pid1; // continue child1 pid1
                    kill(pid1, SIGCONT);
                    kill(pid2, SIGSTOP); // stop child2 
                }
                else
                {
                    local = pid2; // getting the pid of the process to be stopped
                    kill(pid2, SIGCONT); // continue pid2
                    kill(pid1, SIGSTOP);  // stop pid1  
                }
                sleep(timeQuantum);
                // wait_time1 += timequantum
                // wait_time2 += timequantum
                // printf("1 and 2 are %d %d \n0", str1[0], str1[1]);   
            }
            //FILE* file;
            //fopen
            //double w = wait_time1;
            //fprintf(fOut,"%lf ", wait_time1);
            //FILE* file;
            //fopen
            //double w = wait_time2;
            //fprintf(fOut,"%lf ", wait_time2);
            while(!str1[0])
            {   // printf("2 ended");  // kill(pid2, SIGSTOP);
                kill(pid1, SIGCONT);
            }
            while(!str1[1])
            {    // printf("1 ended "); // kill(pid1, SIGSTOP);
                kill(pid2, SIGCONT);
            }
            if(str1[0] == 1 || str1[1] == 1) //end of both processes
            {
                //clock_t toc  = clock();
                //double diff = (double)(toc - tic) / (double)NANO;
                //fprintf(fOut,"%lf ", diff);
                exit(1);
            }
            // printf("both endedd");
            // check str1[0] and str1[2]
            // and check which is still -running 
            // put that in some variable z
            // turnaround time for one which has finished
            while(wait(NULL)); // Parent waits for child
        }
    }
    // printf("yyyyy");
    return 0;
}