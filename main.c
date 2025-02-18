#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  
#include <unistd.h> 
#include <errno.h>
#include <time.h>


#define BUF_SIZE 1024

void child(int i){
    pid_t pid = getpid();
    printf("Starting child %d", i);

    printf(": pid = %d\n", pid);

    exit(EXIT_SUCCESS);
}

int checkError(int val, const char *msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

void takeQuiz(){
    printf("use later");
}

void signalHandler(int sig){
    if(sig == SIGCHLD){
        //Created and sent when a child terminates
        //SIGCHLD is a normal signal so it is possible that multiple occur at the same
        //time and only 1 is remembered. We need to clean up after a child exiting
        //We could call wait, but wait hands, use waitpid since we can tell it not to block
        while(waitpid,(-1, NULL, WNOHANG)>0)continue;
        exit(EXIT_SUCCESS);
    }
    if(sig == SIGUSR1){
        printf("Testing SIGUSR1\n");
    }
    if (sig == SIGALRM){
        //Child receives alarm
        printf("Tick!\n");
    }
}

int main(int argc, char *argv[])
{

    pid_t childId[2];
    pid_t pid;
    int i = 0;
    pid_t pPid;

    pid = getpid();

    printf("parent pid = %d\n", pid);

    struct sigaction sa;
    struct itimerval it;
    it.it_interval.tv_sec = 15;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = 1;
    it.it_value.tv_usec = 0;
   
    sigempyset(&sa.sa_mask);
    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;

    pPid = getpid();

    if(sigaction(SIGCHLD, &sa,NULL)==-1){
        perror("Sigaction for sig chld");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGUSR1, &sa,NULL)==-1){
        perror("Sigaction for sig usr1");
        exit(EXIT_FAILURE);
    }


    switch (childId)
    {
    case 1:
        /* code */
        break;
    case -1:
        perror("Fork");
        eixt(EXIT_FAILURE);
    case 0:
        if(sigaction(SIGALRM, *sa, NULL)== -1)
        {
            perror("sigaction for SIGALRM");
            exit(EXIT_FAILURE);
        }
        if(setitimer(ITIMER_REAL, &it, NULL)==-1)
        {
            perror("setitimer");
            exit(EXIT_FAILURE);
        }
        while(1){
            pause();
        }
        break;
    default:
        while(1){
            pause();
        }
        break;
    }
    //child id here


    for (i=0; i<2;i++){
        childId[i]= fork();
        if (childId[i] == 0) {
            child(i+1);
        }
        if(childId[i]==-1){
            perror("fork");exit(EXIT_FAILURE);
        }

    }
    sleep(5);




    const char *quest_file="quest.txt";
    const char *ans_file="ans.txt";

    int quest_fd = checkError(open(quest_file, O_RDONLY),"Qestion text file");
    int ans_fd = checkError(open(ans_file, O_RDONLY),"Answer text file");


    close(quest_fd);
    close(ans_fd);
    
    exit(EXIT_SUCCESS);
}