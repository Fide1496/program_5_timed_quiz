#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  
#include <unistd.h> 
#include <sys/stat.h>
#include <errno.h>
#include <time.h>


#define BUF_SIZE 1024



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

}

int main(int argc, char *argv[])
{

    struct sigaction sa;
    struct itimerval it;

    it.it_interval.tv_sec = 15;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = 1;
    it.it_value.tv_usec = 0;

    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;
    sigempyset(&sa.sa_mask);

    checkError(sigaction(SIGALRM, &sa, NULL), "sigaction");

    checkError(setitimer(ITIMER_REAL, &it, NULL),"Set itimer");

    while(1){
        pause();
        printf("testing pause");
    }







    const char *quest_file="quest.txt";
    const char *ans_file="ans.txt";

    int quest_fd = checkError(open(quest_file, O_RDONLY),"Qestion text file");
    int ans_fd = checkError(open(ans_file, O_RDONLY),"Answer text file");


    close(quest_fd);
    close(ans_fd);
    
    exit(EXIT_SUCCESS);
}