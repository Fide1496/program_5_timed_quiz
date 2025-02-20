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


#define STRING_SIZE 100
volatile sig_atomic_t time_expired = 0;

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
    char response;
    if(sig == SIGINT){
        printf("\nExit (y/n)? ");
        scanf(" %c", &response);
        if (response == 'y' || response == 'Y') {
            exit(EXIT_SUCCESS);
        }
    }
    if(sig == SIGALRM){
        time_expired = 1;
        printf("\nTime's UP!\n");
    }
}

ssize_t bytes_per_line(int fd, char *buffer, size_t max_len) {
    size_t i = 0;
    char c;
    while (i < max_len - 1) {
        ssize_t n = read(fd, &c, 1);
        if (n <= 0) break;
        if (c == '\n') break;
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return i;
}

int main(int argc, char *argv[])
{

    int correct = 0;
    int total = 0;
    int time_expired = 0;

    struct sigaction sa_timer;
    sa_timer.sa_handler = signalHandler;
    sa_timer.sa_flags = 0;
    sigemptyset(&sa_timer.sa_mask);
    sigaction(SIGALRM, &sa_timer, NULL);

    struct sigaction sa_int;
    sa_int.sa_handler = signalHandler;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);

    const char *quest_file="quest.txt";
    const char *ans_file="ans.txt";

    int quest_fd = checkError(open(quest_file, O_RDONLY),"Qestion text file");
    int ans_fd = checkError(open(ans_file, O_RDONLY),"Answer text file");


    printf("You're about to begin a timed quiz. Each question has a 15 sec timer. Press Control+C to exit.\n");
    char question[STRING_SIZE], answer[STRING_SIZE], user_input[STRING_SIZE];

    while (1) {
        // Read question and answer lines and make sure aren't empty character arrays
        if (bytes_per_line(quest_fd, question, STRING_SIZE) <= 0 ||
        bytes_per_line(ans_fd, answer, STRING_SIZE) <= 0) {
            break;  
        }

        // Display question and prompt user for input
        printf("\nQuestion: %s", question);
        printf("\nYour answer: ");
        fflush(stdout);

        set_timer(); 
        time_expired = 0;

        // Read in user input
        ssize_t read_len = read(STDIN_FILENO, user_input, STRING_SIZE - 1);

        // Handle timer running out
        if (time_expired) {
            total++;
            printf("\nTime has elapsed.\n");
            time_expired = 0;
            continue;
        }

        // Handle new line in user input
        user_input[read_len - 1] = '\0';

        // Compare user input to answer from answer file
        if (strcasecmp(user_input, answer) == 0) {
            printf("Correct!\n");
            correct++;
        } else {
            printf("Wrong!\n");
        }
        total++;
    }

    // Print quiz results
    printf("\nQuiz completed. Score: %d/%d\n", correct, total);

    // Close input files
    close(quest_fd);
    close(ans_fd);
    
    exit(EXIT_SUCCESS);
}