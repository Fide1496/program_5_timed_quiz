#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  
#include <unistd.h> 
#include <errno.h>
#include <time.h>
#include <string.h>

volatile sig_atomic_t time_expired = 0;
volatile sig_atomic_t CONTROL_C = 0;

int checkError(int val, const char *msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}
void set_timer() {
    struct itimerval timer;
    timer.it_value.tv_sec = 15;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

void signalHandler(int sig){
    // printf("nExit (y/N)?");
    char response;
    write(STDOUT_FILENO, "\nExit (y/N)? ", 13);
    read(STDIN_FILENO, &response, 1);
    if (response == 'y' || response == 'Y') {
        CONTROL_C = 1;
    }
}

void timer_handler(int signum) {
    time_expired = 1;
    write(STDOUT_FILENO, "\nTime has elapsed!\n", 20);
}

int main(int argc, char *argv[])
{

    int correct = 0;
    int total = 0;

    struct sigaction sa_timer, sa_int;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_handler = timer_handler
;
    sa_int.sa_flags = 0;

    struct itimerval timer;
    timer.it_value.tv_sec = 15;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    const char *quest_file="quest.txt";
    const char *ans_file="ans.txt";

    char question[100], answer[100], user_input[100];
    int quest_fd = checkError(open(quest_file, O_RDONLY),"Qestion text file");
    int ans_fd = checkError(open(ans_file, O_RDONLY),"Answer text file");

    printf("You're about to begin a timed quest. Each questions has a 15 sec timer. Press Control+C to exit.\n");


    while (!CONTROL_C) {
        int q_len = read(quest_fd, question, 100 - 1);
        int a_len = read(ans_fd, answer, 100 - 1);
        if (q_len <= 0 || a_len <= 0) break;
        
        question[q_len] = '\0';
        answer[a_len] = '\0';
        
        write(STDOUT_FILENO, "\nQuestion: ", 11);
        write(STDOUT_FILENO, question, q_len);
        write(STDOUT_FILENO, "\nYour answer: ", 14);
        
        set_timer();
        time_expired = 0;
        
        int read_len = read(STDIN_FILENO, user_input, 100 - 1);
        if (time_expired) {
            continue;
        }
        user_input[read_len - 1] = '\0'; // Remove newline
        
        if (strcmp(user_input, answer) == 0) {
            write(STDOUT_FILENO, "Correct!\n", 9);
            correct++;
        } else {
            write(STDOUT_FILENO, "Wrong!\n", 7);
        }
        total++;
    }
    
    char result[100];
    int len = snprintf(result, 100, "\nQuiz completed. Score: %d/%d\n", correct, total);
    write(STDOUT_FILENO, result, len);

    close(quest_fd);
    close(ans_fd);
    
    exit(EXIT_SUCCESS);
}