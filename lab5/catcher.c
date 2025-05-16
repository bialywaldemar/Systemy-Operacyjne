#define _POSIX_C_SOURCE 200809L


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int mode = 0;
int count = 0;
pid_t sender_pid = 0;

void sigusr1_handler(int sig, siginfo_t *info, void *context) {
    sender_pid = info->si_pid;
    mode = info->si_value.sival_int;
    count++;

    union sigval val;
    val.sival_int = 0;
    sigqueue(sender_pid, SIGUSR1, val);
}

void sigint_ignore(int sig) {

}

void sigint_message(int sig) {
    printf("!!!  CTRL + C  !!!\n");
}

int main(){
    printf("Catcher PID: %d\n", getpid());
    struct sigaction sa;
    sa.sa_sigaction = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }
    while(1){
        pause();
    
        switch(mode) {
            case 1:
                printf("Tryb 1: %d sygnałów\n", count);
                break;
            case 2:
                printf("Tryb 2: Start odliczania \n");
                for (int i = 1; mode == 2; i++) {
                    printf("%d\n", i);
                    sleep(1);
                }
                break;
            case 3:
                signal(SIGINT, SIG_IGN);
                printf("Tryb 3: Ignore Ctrl+C\n");
                break;
            case 4:
                signal(SIGINT, sigint_message);
                printf("Tryb 4: Zmieniono reakcję na Ctrl+C\n");
                break;   
            case 5:
                printf("Tryb 5: Zamykanie catcher \n");
                exit(0);
            }  
    }
    return 0;

}