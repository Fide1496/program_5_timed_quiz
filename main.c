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

#define STRING_SIZE 100

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

void sigHandler(int sig){
    char response;
    printf("\nExit (y/n)?\n");
    scanf(" %c", &response);
    read(STDIN_FILENO, &response, 1);
    if (response == 'y' || response == 'Y') {
        exit(EXIT_SUCCESS);
    }
}

void timer_handler(int signum) {
    time_expired = 1;
    printf("\nTime has elapsed!\n");
}

int main()
{

    int correct = 0;
    int total = 0;

    struct sigaction sa_timer;
    sa_timer.sa_handler = timer_handler;
    sa_timer.sa_flags = 0;
    sigemptyset(&sa_timer.sa_mask);
    sigaction(SIGALRM, &sa_timer, NULL);

    struct sigaction sa_int;
    sa_int.sa_handler = sigHandler;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);

    const char *quest_file="quest.txt";
    const char *ans_file="ans.txt";

    char question[STRING_SIZE], answer[STRING_SIZE], user_input[STRING_SIZE];
    int quest_fd = checkError(open(quest_file, O_RDONLY),"Open question text file to read");
    int ans_fd = checkError(open(ans_file, O_RDONLY),"Open answer text file to read");

    printf("You're about to begin a timed quest. Each questions has a 15 sec timer. Press Control+C to exit.\n");

    char question[STRING_SIZE], answer[STRING_SIZE], user_input[STRING_SIZE];

    while (1) {
        int quest_len = checkError(read(quest_fd, question, STRING_SIZE - 1),"Checking Read");
        int ans_len = checkError(read(ans_fd, answer, STRING_SIZE - 1),"Checking Read");
        if (quest_len <= 0 || ans_len <= 0) break;
        
        question[quest_len] = '\0';
        answer[ans_len] = '\0';
        
        printf("Question: %s", question);
        write(STDOUT_FILENO, question, quest_len);
        printf("\nYour answer: ");
        
        set_timer();
        time_expired = 0;
        
        int read_len = read(STDIN_FILENO, user_input, STRING_SIZE - 1);
        if (time_expired) {
            continue;
        }
        user_input[read_len - 1] = '\0';
        
        if (strcmp(user_input, answer) == 0) {
            printf("Correct!");
            correct++;
        } else {
            printf("WRONG!");
        }
        total++;
    }
    
    char result[STRING_SIZE];
    int len = snprintf(result, STRING_SIZE, "\nQuiz completed. Score: %d/%d\n", correct, total);
    write(STDOUT_FILENO, result, len);

    close(quest_fd);
    close(ans_fd);
    
    exit(EXIT_SUCCESS);
}