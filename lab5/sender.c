#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int confirmed = 0;
void confirmation_handler(int sig) {
    confirmed = 1;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s\n", argv[0]);
        exit(1);
    }

    pid_t catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    signal(SIGUSR1, confirmation_handler);

    union sigval val;
    val.sival_int = mode;

    if (sigqueue(catcher_pid, SIGUSR1, val) < 0) {
        perror("sigqueue");
        exit(1);
    }

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    while (!confirmed)
        sigsuspend(&oldmask);

    printf("wyslano\n");
    return 0;
}