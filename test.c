#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define STRING_SIZE 100

volatile sig_atomic_t time_expired = 0;

void set_timer() {
    struct itimerval timer;
    timer.it_value.tv_sec = 15;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

void sigHandler(int sig) {
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
        printf("\nTime has elapsed!\n");
    }
}
    


// void timer_handler(int signum) {
//     time_expired = 1;
//     printf("\nTime has elapsed!\n");
// }

int main() {
    int correct = 0, total = 0;


    struct sigaction sa_timer;
    sa_timer.sa_handler = sigHandler;
    sa_timer.sa_flags = 0;
    sigemptyset(&sa_timer.sa_mask);
    sigaction(SIGALRM, &sa_timer, NULL);

    struct sigaction sa_int;
    sa_int.sa_handler = sigHandler;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);

    const char *quest_file = "quest.txt";
    const char *ans_file = "ans.txt";

    FILE *quest_fp = fopen(quest_file, "r");
    FILE *ans_fp = fopen(ans_file, "r");

    printf("You're about to begin a timed quiz. Each question has a 15 sec timer. Press Control+C to exit.\n");
    char question[STRING_SIZE], answer[STRING_SIZE], user_input[STRING_SIZE];

    while (fgets(question, STRING_SIZE, quest_fp) && fgets(answer, STRING_SIZE, ans_fp)) {
        question[strcspn(question, "\n")] = 0;
        answer[strcspn(answer, "\n")] = 0; 

        printf("\nQuestion: %s\nYour answer: ", question);
        
        set_timer();  
        time_expired = 0;
        
        if (fgets(user_input, STRING_SIZE, stdin) == NULL || time_expired) {
            printf("\nSkipping question due to timeout.\n");
            continue;
        }

        user_input[strcspn(user_input, "\n")] = 0;

        if (strcasecmp(user_input, answer) == 0) {
            printf("Correct!\n");
            correct++;
        } else {
            printf("Wrong!\n");
        }
        total++;
    }

    printf("\nQuiz completed. Score: %d/%d\n", correct, total);

    fclose(quest_fp);
    fclose(ans_fp);

    return 0;
}
